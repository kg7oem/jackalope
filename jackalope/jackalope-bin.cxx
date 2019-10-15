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
#include <jackalope/jackalope.h>
#include <jackalope/log/dest.h>
#include <jackalope/logging.h>

using namespace jackalope;

struct dev_node : public audio_node_t {
    dev_node(const string_t& name_in)
    : audio_node_t(name_in)
    { }

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

    dev_node foo("node 1");
    auto& foo_input = foo.add_input("pcm[real]", "test");

    dev_node bar("node 2");
    auto& bar_output = bar.add_output("pcm[real]", "fiddle");

    dev_node blah("node 3");
    auto& blah_output = blah.add_output("pcm[real]", "booooze");

    bar_output.link(foo_input);
    blah_output.link(foo_input);

    bar_output.notify();
    blah_output.notify();

    foo.add_property("config:blah", property_t::type_t::integer);
    log_info("Property value: ", foo.get_property("config:blah").get_real());

    return(0);
}
