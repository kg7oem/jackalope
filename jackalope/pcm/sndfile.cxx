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

#include <jackalope/string.h>
#include <jackalope/jackalope.h>
#include <jackalope/logging.h>
#include <jackalope/pcm/sndfile.h>

namespace jackalope {

namespace pcm {

static shared_t<sndfile_node_t> sndfile_object_constructor(const init_list_t& init_list_in)
{
    return jackalope::make_shared<sndfile_node_t>(init_list_in);
}

void sndfile_init()
{
    add_node_constructor(JACKALOPE_PCM_SNDFILE_CLASS, sndfile_object_constructor);
}

sndfile_node_t::sndfile_node_t(const init_list_t& init_list_in)
: pcm_node_t(init_list_in)
{ }

sndfile_node_t::~sndfile_node_t()
{
    if (source_buffer != nullptr) {
        delete source_buffer;
    }

    if (source_file != nullptr) {
        close_file(source_file);
        source_file = nullptr;
    }

    if (io_thread != nullptr) {
        io_thread->join();
        delete io_thread;
        io_thread = nullptr;
    }
}

void sndfile_node_t::init()
{
    assert_lockable_owner();

    pcm_node_t::init();

    add_property(JACKALOPE_PCM_SNDFILE_CONFIG_PATH, property_t::type_t::string, init_args);

    add_signal("file.eof");

}

void sndfile_node_t::activate()
{
    assert_lockable_owner();

    pcm_node_t::activate();

    if (sinks.size() > 0) {
        throw_runtime_error("sndfile does not support having sinks");
    }

    auto source_file_name = get_property(JACKALOPE_PCM_SNDFILE_CONFIG_PATH)->get();
    source_file = sndfile::sf_open(source_file_name.c_str(), sndfile::SFM_READ, &source_info);

    if (source_file == nullptr) {
        throw_runtime_error("Could not open ", source_file_name, ": ", sndfile::sf_strerror(nullptr));
    }

    int sample_rate = get_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE)->get_size();

    if (source_info.samplerate != sample_rate) {
        throw_runtime_error("File sample rate did not match node sample rate: ", source_info.samplerate);
    }

    auto sources_count = sources.size();
    size_t channel_count = source_info.channels;

    if (sources_count == channel_count && channel_count != 1) {
        throw_runtime_error("number of sources and channels must match or channels must be 1");
    }

    // auto buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();
    // source_buffer = new real_t[source_info.channels * buffer_size];

    // for(auto i : outputs) {
    //     auto pcm_output = dynamic_pointer_cast<pcm_real_output_t>(i);
    //     pcm_output->set_num_samples(buffer_size);
    // }
}

void sndfile_node_t::close_file(sndfile_handle_t * file_in)
{
    auto result = sndfile::sf_close(file_in);

    if (result != 0) {
        throw_runtime_error("Could not close sndfile: ", sndfile::sf_strerror(nullptr));
    }
}

void sndfile_node_t::start()
{
    assert_lockable_owner();
    assert(io_thread == nullptr);

    object_t::start();

    io_thread = new thread_t(std::bind(&sndfile_node_t::io_thread_handler, this));
}

void sndfile_node_t::io_thread_handler()
{
    while(1) {
        size_t buffer_size = 0;

        {
            auto lock = get_object_lock();
            buffer_size =  get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();
        }

        size_t num_channels = source_info.channels;
        pcm_buffer_t<real_t> sndfile_buffer(num_channels * buffer_size);
        auto sndfile_buffer_ptr = sndfile_buffer.get_pointer();

        assert(source_file != nullptr);
        size_t frames_read = sndfile::sf_readf_float(source_file, sndfile_buffer_ptr, buffer_size);

        if (frames_read == 0) {
            close_file(source_file);

            auto lock = get_object_lock();
            get_signal(JACKALOPE_SIGNAL_FILE_EOF)->send();
            return;
        }

        pool_vector_t<shared_t<pcm_buffer_t<real_t>>> buffer_list;

        for(size_t i = 0; i < num_channels; i++) {
            auto source_buffer = jackalope::make_shared<pcm_buffer_t<real_t>>(buffer_size);

            pcm_extract_interleave(sndfile_buffer_ptr, source_buffer->get_pointer(), i, num_channels, frames_read);
            buffer_list.push_back(source_buffer);
        }

        {
            auto lock = get_object_lock();

            if (num_channels == 1) {
                auto pcm_buffer = buffer_list.front();
                for(auto i : get_sources()) {
                    if (i->type != JACKALOPE_CHANNEL_TYPE_PCM_REAL) {
                        throw_runtime_error("Unexpected channel type: ", i->type);
                    }

                    auto pcm_source = dynamic_pointer_cast<pcm_source_t<real_t>>(i);
                    pcm_source->set_buffer(pcm_buffer);
                }
            } else {
                for(size_t i = 0; i < num_channels; i++) {
                    auto source = get_source(i);

                    if (source->type != JACKALOPE_CHANNEL_TYPE_PCM_REAL) {
                        throw_runtime_error("Unexpected channel type: ", source->type);
                    }

                    auto pcm_source = dynamic_pointer_cast<pcm_source_t<real_t>>(source);
                    pcm_source->set_buffer(buffer_list[i]);
                }
            }
        }
    }
}

// void sndfile_node_t::pcm_ready()
// {
//     pcm_node_t::pcm_ready();

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
