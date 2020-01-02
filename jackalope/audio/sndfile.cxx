// Jackalope Audio Engine
// Copyright 2019 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/audio/sndfile.h>
#include <jackalope/string.h>
#include <jackalope/jackalope.h>
#include <jackalope/logging.h>

namespace jackalope {

namespace audio {

static shared_t<sndfile_node_t> sndfile_object_constructor(const init_list_t& init_list_in)
{
    return jackalope::make_shared<sndfile_node_t>(init_list_in);
}

void sndfile_init()
{
    add_object_constructor(JACKALOPE_AUDIO_SNDFILE_OBJECT_TYPE, sndfile_object_constructor);
}

sndfile_node_t::sndfile_node_t(const init_list_t& init_list_in)
: node_t(init_list_in)
{
    assert(type == JACKALOPE_AUDIO_SNDFILE_OBJECT_TYPE);
}

sndfile_node_t::~sndfile_node_t()
{
    if (source_file != nullptr) {
        close_file();
    }
}

void sndfile_node_t::init()
{
    assert_lockable_owner();

    node_t::init();

    add_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_CONFIG_PATH, property_t::type_t::string, init_args);

    // add_signal("file.eof");
}

void sndfile_node_t::activate()
{
    assert_lockable_owner();

    node_t::activate();

    if (sinks.size() > 0) {
        throw_runtime_error("sndfile does not support having sinks");
    }

    auto source_file_name = get_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_CONFIG_PATH)->get();
    source_file = sndfile::sf_open(source_file_name.c_str(), sndfile::SFM_READ, &source_info);

    if (source_file == nullptr) {
        throw_runtime_error("Could not open ", source_file_name, ": ", sndfile::sf_strerror(nullptr));
    }

    auto sample_rate_property = get_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE);

    if (sample_rate_property->is_defined()) {
        int sample_rate = sample_rate_property->get_size();
        if (source_info.samplerate != sample_rate) {
            throw_runtime_error("File sample rate did not match node sample rate: ", source_info.samplerate);
        }
    } else {
        sample_rate_property->set(source_info.samplerate);
    }

    size_t channel_count = source_info.channels;

    for (size_t i = 0; i < channel_count; i++) {
        auto source_name = to_string("Output ", i + 1);
        add_source(source_name, JACKALOPE_TYPE_AUDIO);
    }
}

void sndfile_node_t::close_file()
{
    auto result = sndfile::sf_close(source_file);
    source_file = nullptr;

    if (result != 0) {
        throw_runtime_error("Could not close sndfile: ", sndfile::sf_strerror(nullptr));
    }
}

void sndfile_node_t::start()
{
    assert_lockable_owner();

    object_t::start();
}

bool sndfile_node_t::should_run()
{
    assert_lockable_owner();

    for (auto i : sources) {
        if (! i->is_available()) {
            return false;
        }
    }

    return true;
}

void sndfile_node_t::run()
{
    assert_lockable_owner();

    assert(source_file != nullptr);
    assert(sources_known_available);

    size_t buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();
    size_t channels = source_info.channels;

    audio_buffer_t sndfile_buffer(buffer_size * channels);

    size_t frames_read = sndfile::sf_readf_float(source_file, sndfile_buffer.get_pointer(), buffer_size);

    NODE_LOG(info, "sndfile read: ", frames_read);

    if (frames_read == 0) {
        stop();
        return;
    }

    for(int i = 0; i < source_info.channels; i++) {
        auto source = get_source(i)->shared_obj<audio_source_t>();
        auto source_buffer = jackalope::make_shared<audio_buffer_t>(buffer_size);

        pcm_extract_interleave(sndfile_buffer.get_pointer(), source_buffer->get_pointer(), i, source_info.channels, frames_read);
        source->notify_buffer(source_buffer);
    }
}

void sndfile_node_t::stop()
{
    assert_lockable_owner();

    close_file();

    node_t::stop();
}

// void sndfile_node_t::io_thread_handler()
// {
//     while(1) {
//         size_t buffer_size = 0;

//         {
//             auto lock = get_object_lock();
//             buffer_size =  get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();
//         }

//         size_t num_channels = source_info.channels;
//         pcm_buffer_t<real_t> sndfile_buffer(num_channels * buffer_size);
//         auto sndfile_buffer_ptr = sndfile_buffer.get_pointer();

//         assert(source_file != nullptr);
//         size_t frames_read = sndfile::sf_readf_float(source_file, sndfile_buffer_ptr, buffer_size);

//         if (frames_read == 0) {
//             close_file(source_file);

//             auto lock = get_object_lock();
//             get_signal(JACKALOPE_SIGNAL_FILE_EOF)->send();
//             return;
//         }

//         pool_vector_t<shared_t<pcm_buffer_t<real_t>>> buffer_list;

//         for(size_t i = 0; i < num_channels; i++) {
//             auto source_buffer = jackalope::make_shared<pcm_buffer_t<real_t>>(buffer_size);

//             pcm_extract_interleave(sndfile_buffer_ptr, source_buffer->get_pointer(), i, num_channels, frames_read);
//             buffer_list.push_back(source_buffer);
//         }

//         {
//             auto lock = get_object_lock();

//             if (num_channels == 1) {
//                 auto pcm_buffer = buffer_list.front();
//                 for(auto i : get_sources()) {
//                     if (i->type != JACKALOPE_CHANNEL_TYPE_PCM_REAL) {
//                         throw_runtime_error("Unexpected channel type: ", i->type);
//                     }

//                     auto pcm_source = dynamic_pointer_cast<pcm_real_source_t>(i);
//                     // pcm_source->set_buffer(pcm_buffer);
//                 }
//             } else {
//                 for(size_t i = 0; i < num_channels; i++) {
//                     auto source = get_source(i);

//                     if (source->type != JACKALOPE_CHANNEL_TYPE_PCM_REAL) {
//                         throw_runtime_error("Unexpected channel type: ", source->type);
//                     }

//                     auto pcm_source = dynamic_pointer_cast<pcm_real_source_t>(source);
//                     // pcm_source->set_buffer(buffer_list[i]);
//                 }
//             }
//         }
//     }
// }

// void sndfile_node_t::pcm_ready()
// {
//     audio_node_t::pcm_ready();

//     for(auto i : outputs) {
//         auto pcm_output = dynamic_pointer_cast<pcm_real_output_t>(i);
//         pcm_output->zero_buffer();
//         pcm_output->set_dirty();
//     }

//     if (source_file != nullptr) {
//         size_t frames_read = sndfile::sf_readf_float(source_file, source_buffer, get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE).get_size());

//         // log_info("Got ", frames_read, " frames from sndlib");

//         if (frames_read == 0) {
//             close_file(source_file);
//             source_file = nullptr;

//             get_signal(JACKALOPE_SIGNAL_FILE_EOF)->send();
//         }

//         for (size_t i = 0; i < outputs.size(); i++) {
//             auto pcm_output = dynamic_pointer_cast<pcm_real_output_t>(outputs[i]);
//             auto dest_buffer = pcm_output->get_buffer_pointer();
//             auto buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE).get_size();
//             pcm_extract_interleaved_channel(source_buffer, dest_buffer, i, source_info.channels, buffer_size);
//         }
//     }

//     notify();
// }

} // namespace pcm

} // namespace jackalope
