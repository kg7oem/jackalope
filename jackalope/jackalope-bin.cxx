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
#include <jackalope/jackalope.h>
#include <jackalope/log/dest.h>
#include <jackalope/logging.h>

using namespace jackalope;

struct dev_node : public audio_node_t {
    dev_node(const string_t& name_in)
    : audio_node_t(name_in, "devnode")
    { }

    virtual void activate()
    {
        audio_node_t::activate();
    }

    virtual void start()
    {
        audio_node_t::start();
    }

    virtual property_t& add_property(const string_t& name_in, property_t::type_t type_in)
    {
        return audio_node_t::add_property(name_in, type_in);
    }

    virtual property_t& get_property(const string_t& name_in)
    {
        return audio_node_t::get_property(name_in);
    }

    virtual input_t& add_input(const string_t& class_in, const string_t& name_in)
    {
        return audio_node_t::add_input(class_in, name_in);
    }

    virtual output_t& add_output(const string_t& class_in, const string_t& name_in)
    {
        return audio_node_t::add_output(class_in, name_in);
    }
};

int main(void)
{
    auto dest = make_shared<log::console_dest_t>(log::level_t::info);
    log::get_engine()->add_destination(dest);

    jackalope_init();

    audio_domain_t domain("main");
    domain.get_property(JACKALOPE_AUDIO_PROPERTY_SAMPLE_RATE).set(48000);
    domain.get_property(JACKALOPE_AUDIO_PROPERTY_BUFFER_SIZE).set(128);
    auto& domain_output = domain.add_output(JACKALOPE_PCM_CHANNEL_CLASS_REAL, "some output");
    domain.add_input(JACKALOPE_PCM_CHANNEL_CLASS_REAL, "input 1");
    domain.add_input(JACKALOPE_PCM_CHANNEL_CLASS_REAL, "input 2");
    domain.init();
    domain.activate();
    domain.start();

    auto& node1 = domain.make_node(JACKALOPE_AUDIO_LADSPA_CLASS, "node 1");
    node1.get_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_FILE).set("/usr/local/lib/ladspa/ZamComp-ladspa.so");
    node1.init();
    node1.activate();
    node1.start();

    auto& node2 = domain.make_node(JACKALOPE_AUDIO_LADSPA_CLASS, "node 2");
    node2.get_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_FILE).set("/usr/local/lib/ladspa/ZamComp-ladspa.so");
    node2.init();
    node2.activate();
    node2.start();

    domain_output.link(node1.get_input("Audio Input 1"));
    domain_output.link(node2.get_input("Audio Input 1"));
    node1.get_output("Audio Output 1").link(domain.get_input("input 1"));
    node2.get_output("Audio Output 1").link(domain.get_input("input 2"));

    domain_output.notify();

    return(0);
}
