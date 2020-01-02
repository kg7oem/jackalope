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
    if (io_thread != nullptr) {
        io_thread->join();
        delete io_thread;
        io_thread = nullptr;
    }

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
    auto read_ahead = add_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_AHEAD, property_t::type_t::size, init_args);
    auto read_size = add_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_SIZE, property_t::type_t::size, init_args);

    if (! read_ahead->is_defined()) {
        read_ahead->set(JACKALOPE_AUDIO_SNDFILE_DEFAULT_READ_AHEAD);
    }

    if (! read_size->is_defined()) {
        read_size->set(JACKALOPE_AUDIO_SNDFILE_DEFAULT_READ_SIZE);
    }
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

    if (result != 0) {
        throw_runtime_error("Could not close sndfile: ", sndfile::sf_strerror(nullptr));
    }

    source_file = nullptr;
}

void sndfile_node_t::start()
{
    assert_lockable_owner();

    node_t::start();

    io_thread = new thread_t(std::bind(&sndfile_node_t::be_io_thread, this));
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

void sndfile_node_t::be_io_thread()
{
    while(true) {
        auto lock = get_object_lock();

        if (source_file == nullptr) {
            return;
        }

        auto read_ahead = get_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_AHEAD)->get_size();
        auto read_size = get_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_SIZE)->get_size();
        auto buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();
        size_t num_channels = source_info.channels;
        auto min_thread_work_size = read_ahead / buffer_size;

        NODE_LOG(info, "read_ahead: ", read_ahead, "; read_size: ", read_size, "; min_thread_work_size: ", min_thread_work_size);

        NODE_LOG(info, "waiting for sndfile io thread to have work to do");
        thread_work_cond.wait(lock, [&] { return stopped_flag || thread_work.size() < min_thread_work_size; });
        NODE_LOG(info, "sndfile io thread woke up");

        if (stopped_flag) {
            NODE_LOG(info, "sndfile io node is exiting because the node is stopped");
            return;
        }

        auto buffer = jackalope::make_shared<audio_buffer_t>(num_channels * read_size);
        assert(source_file != nullptr);
        size_t frames_read = sndfile::sf_readf_float(source_file, buffer->get_pointer(), read_size);
        NODE_LOG(info, "sndfile io thread read: ", frames_read);

        if (frames_read == 0) {
            close_file();
            thread_work.push_back(nullptr);
        } else {
            auto samples_left = frames_read;
            auto p = buffer->get_pointer();

            while(samples_left > 0) {
                auto buffer = jackalope::make_shared<audio_buffer_t>(num_channels * buffer_size);
                auto copy_num = num_channels * buffer_size;

                if (samples_left < copy_num) {
                    copy_num = samples_left;
                }

                pcm_copy(p, buffer->get_pointer(), copy_num);
                thread_work.push_back(buffer);

                samples_left -= copy_num;
                p += copy_num;
            }
        }

        thread_work_cond.notify_all();
    }
}

void sndfile_node_t::run()
{
    assert_lockable_owner();

    NODE_LOG(info, "waiting for work from sndfile io thread");
    thread_work_cond.wait(object_mutex, [this] { return stopped_flag || thread_work.size() > 0; });
    assert_lockable_owner();
    NODE_LOG(info, "done waiting for sndfile io thread");

    if (stopped_flag) {
        return;
    }

    auto buffer = thread_work.front();
    thread_work.pop_front();

    thread_work_cond.notify_all();

    if (buffer == nullptr) {
        NODE_LOG(info, "got EOF from sndfile io thread");
        stop();
        return;
    }

    auto buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();

    for(int i = 0; i < source_info.channels; i++) {
        auto source_buffer = jackalope::make_shared<audio_buffer_t>(buffer_size);
        auto source = get_source(i)->shared_obj<audio_source_t>();

        pcm_extract_interleave(buffer->get_pointer(), source_buffer->get_pointer(), i, source_info.channels, buffer_size);
        source->notify_buffer(source_buffer);
    }
}

void sndfile_node_t::stop()
{
    assert_lockable_owner();

    node_t::stop();

    thread_work_cond.notify_all();
}

} // namespace pcm

} // namespace jackalope
