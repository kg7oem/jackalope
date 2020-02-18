// Jackalope Audio Engine
// Copyright 2020 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#include <jackalope/audio/channel.h>
#include <jackalope/audio/sndfile.h>
#include <jackalope/logging.h>

#define JACKALOPE_AUDIO_SNDFILE_DEFAULT_READ_AHEAD          262144 // bytes
#define JACKALOPE_AUDIO_SNDFILE_DEFAULT_READ_SIZE           16384 // bytes
#define JACKALOPE_AUDIO_SNDFILE_OBJECT_TYPE                 "audio::sndfile"
#define JACKALOPE_AUDIO_SNDFILE_PROPERTY_CONFIG_PATH        "config.path"
#define JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_AHEAD         "config.read_ahead"
#define JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_SIZE          "config.read_size"

namespace jackalope {

namespace audio {

shared_t<plugin_t> sndfile_plugin_t::make(shared_t<project_t> project_in, const init_args_t& init_args_in)
{
    return object_t::make<sndfile_plugin_t>(project_in, init_args_in);
}

sndfile_plugin_t::sndfile_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in)
: driver_plugin_t(project_in, init_args_in)
{ }

sndfile_plugin_t::~sndfile_plugin_t()
{
    if (source_file != nullptr) {
        close_file();
    }
}

const string_t& sndfile_plugin_t::get_type()
{
    return type;
}

void sndfile_plugin_t::will_init()
{
    assert_lockable_owner();

    add_channel_type(audio_channel_info_t::type);

    add_properties({
        { JACKALOPE_AUDIO_SNDFILE_PROPERTY_CONFIG_PATH, property_t::type_t::string },
        { JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_AHEAD, property_t::type_t::size },
        { JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_SIZE, property_t::type_t::size },
    });

    driver_plugin_t::will_init();
}

void sndfile_plugin_t::did_init()
{
    assert_lockable_owner();

    set_undef_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_AHEAD, JACKALOPE_AUDIO_SNDFILE_DEFAULT_READ_AHEAD);
    set_undef_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_SIZE, JACKALOPE_AUDIO_SNDFILE_DEFAULT_READ_SIZE);

    driver_plugin_t::did_init();
}

void sndfile_plugin_t::will_activate()
{
    assert_lockable_owner();

    add_init_args_channels();

    if (sinks.size() > 0) {
        throw_runtime_error("sndfile plugin can not have sinks");
    }

    open_file();

    driver_plugin_t::will_activate();
}

void sndfile_plugin_t::will_stop()
{
    assert_lockable_owner();

    if (source_file != nullptr) {
        close_file();
    }

    driver_plugin_t::will_stop();
}

void sndfile_plugin_t::open_file()
{
    assert_lockable_owner();

    auto read_ahead_prop = get_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_AHEAD);
    auto read_size_prop = get_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_READ_SIZE);
    auto buffer_size_prop = get_property(JACKALOPE_PROPERTY_AUDIO_BUFFER_SIZE);
    auto path_prop = get_property(JACKALOPE_AUDIO_SNDFILE_PROPERTY_CONFIG_PATH);
    auto sample_rate_property = get_property(JACKALOPE_PROPERTY_AUDIO_SAMPLE_RATE);

    object_log_trace("readahead: ", read_ahead_prop->get_size(), "; read size: ", read_size_prop->get_size());

    if (read_ahead_prop->get_size() < read_size_prop->get_size()) {
        throw_runtime_error("readahead must be greater than or equal to read size", read_ahead_prop);
    } else if (read_ahead_prop->get_size() % read_size_prop->get_size() != 0) {
        throw_runtime_error("readahead must be an even multiple of read size");
    }

    auto source_file_name = path_prop->get();
    source_file = sndfile::sf_open(source_file_name.c_str(), sndfile::SFM_READ, &source_info);

    if (source_file == nullptr) {
        throw_runtime_error("Could not open ", source_file_name, ": ", sndfile::sf_strerror(nullptr));
    }

    if (sample_rate_property->is_defined()) {
        int sample_rate = sample_rate_property->get_size();
        if (source_info.samplerate != sample_rate) {
            throw_runtime_error("File sample rate did not match node sample rate: ", source_info.samplerate);
        }
    } else {
        sample_rate_property->set(source_info.samplerate);
    }

    size_t channel_count = source_info.channels;
    size_t sources_count = sources.size();

    if (sources_count == 0) {
        for (size_t i = 0; i < channel_count; i++) {
            auto source_name = to_string("Output ", i + 1);
            add_source(source_name, audio_channel_info_t::type);
        }
    } else if (sources_count != channel_count) {
        throw_runtime_error("sources count did not match channel count from file");
    }
}

void sndfile_plugin_t::close_file()
{
    assert_lockable_owner();

    if (source_file != nullptr) {
        auto result = sndfile::sf_close(source_file);

        if (result != 0) {
            throw_runtime_error("Could not close sndfile: ", sndfile::sf_strerror(nullptr));
        }

        source_file = nullptr;
    }
}

void sndfile_plugin_t::execute()
{
    assert_lockable_owner();

    object_log_trace("sndfile node is executing");

    auto buffer_size = get_property("audio.buffer_size")->get_size();
    auto num_sources = sources.size();
    auto sndfile_buffer_size_samples = source_info.channels * buffer_size;

    auto sndfile_buffer = audio_buffer_t::make(sndfile_buffer_size_samples);
    size_t frames_read = sndfile::sf_readf_float(source_file, sndfile_buffer->get_pointer(), buffer_size);

    if (frames_read == 0) {
        object_log_info("Got EOF from sndfile");
        stop();
        return;
    }

    assert((int)num_sources == source_info.channels);
    for(size_t i = 0; i < num_sources; i++) {
        auto source_buffer = audio_buffer_t::make(buffer_size);

        pcm_extract_interleave(sndfile_buffer->get_pointer(), source_buffer->get_pointer(), i, source_info.channels, frames_read);
        get_source<audio_source_t>(i)->notify_buffer(source_buffer);
    }
}

} // namespace audio

} //namespace jackalope
