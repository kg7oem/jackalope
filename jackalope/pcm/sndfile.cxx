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

    auto source_file_name = get_property(JACKALOPE_PCM_SNDFILE_CONFIG_PATH)->get();
    source_file = sndfile::sf_open(source_file_name.c_str(), sndfile::SFM_READ, &source_info);

    if (source_file == nullptr) {
        throw_runtime_error("Could not open ", source_file_name, ": ", sndfile::sf_strerror(nullptr));
    }

    int sample_rate = get_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE)->get_size();

    if (source_info.samplerate != sample_rate) {
        throw_runtime_error("File sample rate did not match node sample rate: ", source_info.samplerate);
    }

    // for(int i = 0; i < source_info.channels; i++) {
    //     add_source(to_string("output ", i + 1), JACKALOPE_PCM_CHANNEL_TYPE_REAL);
    // }

    auto buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();
    source_buffer = new real_t[source_info.channels * buffer_size];

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
        auto lock = get_object_lock();
        assert(source_file != nullptr);

        auto buffer_size =  get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();
        size_t frames_read = sndfile::sf_readf_float(source_file, source_buffer, buffer_size);

        if (frames_read == 0) {
            close_file(source_file);
            get_signal(JACKALOPE_SIGNAL_FILE_EOF)->send();
            return;
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
