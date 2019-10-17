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

#include <iostream>
#include <string>

#include <jackalope/audio.h>
#include <jackalope/audio/ladspa.h>
#include <jackalope/audio/portaudio.h>
#include <jackalope/audio/sndfile.h>
#include <jackalope/jackalope.h>
#include <jackalope/log/dest.h>
#include <jackalope/logging.h>

#define BUFFER_SIZE 128
#define SAMPLE_RATE 48000
#define LADSPA_ZAMTUBE_ID 1515476290

using namespace jackalope;

real_t ** make_domain_buffer(const size_t num_channels_in, const size_t num_samples_in)
{
    real_t ** buffer;

    buffer = new real_t * [num_channels_in];
    for(size_t i = 0; i < num_channels_in; i++) {
        buffer[i] = new real_t [num_samples_in];
    }

    return buffer;
}

int main(void)
{
    auto dest = make_shared<log::console_dest_t>(log::level_t::info);
    log::get_engine()->add_destination(dest);

    jackalope_init();

    audio_domain_t domain("main domain");
    domain.get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE).set(SAMPLE_RATE);
    domain.get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).set(BUFFER_SIZE);
    domain.init();
    domain.add_input(JACKALOPE_PCM_CHANNEL_CLASS_REAL, "left input");
    domain.add_input(JACKALOPE_PCM_CHANNEL_CLASS_REAL, "right input");
    domain.activate();

    auto driver = new audio::portaudio_driver_t("main driver", &domain);
    driver->init();
    driver->activate();

    auto& left_tube = domain.make_node(JACKALOPE_AUDIO_LADSPA_CLASS, "left tube");
    left_tube.get_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_ID).set(LADSPA_ZAMTUBE_ID);
    left_tube.init();
    left_tube.activate();
    left_tube.start();

    auto& right_tube = domain.make_node(JACKALOPE_AUDIO_LADSPA_CLASS, "right tube");
    right_tube.get_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_ID).set(LADSPA_ZAMTUBE_ID);
    right_tube.init();
    right_tube.activate();
    right_tube.start();

    auto& file = domain.make_node(JACKALOPE_AUDIO_SNDFILE_CLASS, "sound file");
    file.init();
    file.get_property("config:source").set("/usr/share/sounds/alsa/Rear_Right.wav");
    file.activate();
    file.start();

    file.get_output("output 1").link(left_tube.get_input("Audio Input 1"));
    file.get_output("output 1").link(right_tube.get_input("Audio Input 1"));
    left_tube.get_output("Audio Output 1").link(domain.get_input("left input"));
    right_tube.get_output("Audio Output 1").link(domain.get_input("right input"));

    driver->start();

    while(1) {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

    return(0);
}
