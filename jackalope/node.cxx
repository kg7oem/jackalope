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

#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/node.h>

namespace jackalope {

node_t::node_t(const string_t& name_in, const string_t& class_name_in)
: name(name_in), class_name(class_name_in)
{
    add_property(JACKALOPE_NODE_PROPERTY_NAME, property_t::type_t::string).set(name_in);
    add_property(JACKALOPE_NODE_PROPERTY_CLASS, property_t::type_t::string).set(class_name_in);
}

node_t::~node_t()
{
    for (auto i : inputs) {
        delete i.second;
    }

    for (auto i : outputs) {
        delete i.second;
    }
}

void node_t::init()
{
    if (initialized_flag) {
        throw_runtime_error("Can not initalize a node that has already been initialized");
    }

    initialized_flag = true;
}

void node_t::activate()
{
    if (! initialized_flag) {
        throw_runtime_error("Can not activate a node that is not initalized");
    }

    if (activated_flag) {
        throw_runtime_error("can not activate a node that has already been activated");
    }

    activated_flag = true;
}

void node_t::start()
{
    if (! activated_flag) {
        throw_runtime_error("can not start a node that has not been activated");
    }

    if (started_flag) {
        throw_runtime_error("can not start a node that has already been started");
    }

    started_flag = true;
}

bool node_t::is_initialized()
{
    return initialized_flag;
}

bool node_t::is_activated()
{
    return activated_flag;
}

bool node_t::is_started()
{
    return started_flag;
}

const string_t& node_t::get_name()
{
    return name;
}

const string_t& node_t::get_class_name()
{
    return class_name;
}

property_t& node_t::add_property(const string_t& name_in, property_t::type_t type_in)
{
    if (activated_flag) {
        throw_runtime_error("can not add a property to a node that has been activated");
    }

    auto result = properties.emplace(std::make_pair(name_in, type_in));

    if (! result.second) {
        throw_runtime_error("Attempt to add duplicate property name: ", name_in);
    }

    return result.first->second;
}

property_t& node_t::get_property(const string_t& name_in)
{
    auto found = properties.find(name_in);

    if (found == properties.end()) {
        throw_runtime_error("Could not find property: ", name_in);
    }

    return found->second;
}

input_t& node_t::add_input(const string_t& channel_class_in, const string_t& name_in)
{
    if (activated_flag) {
        throw_runtime_error("Can not add an input to a node that has been activated");
    }

    if (inputs.find(channel_class_in) != inputs.end()) {
        throw_runtime_error("duplicate input name: ", name_in);
    }

    auto property_name = vaargs_to_string("input:", name_in);

    if (properties.find(property_name) != properties.end()) {
        throw_runtime_error("property existed for new input: ", property_name);
    }

    add_property(property_name, property_t::type_t::string).set(channel_class_in);

    auto new_channel = make_input_channel(channel_class_in, name_in, *this);

    inputs[name_in] = new_channel;

    return *new_channel;
}

input_t& node_t::get_input(const string_t& name_in)
{
    auto found = inputs.find(name_in);

    if (found == inputs.end()) {
        throw_runtime_error("Could not find input: ", name_in);
    }

    return *found->second;
}

output_t& node_t::get_output(const string_t& name_in)
{
    auto found = outputs.find(name_in);

    if (found == outputs.end()) {
        throw_runtime_error("Could not find input: ", name_in);
    }

    return *found->second;
}

output_t& node_t::add_output(const string_t& channel_class_in, const string_t& name_in)
{
    if (activated_flag) {
        throw_runtime_error("Can not add an output to a node that has been activated");
    }

    if (inputs.find(channel_class_in) != inputs.end()) {
        throw_runtime_error("duplicate input name: ", name_in);
    }

    auto property_name = vaargs_to_string("output:", name_in);

    if (properties.find(property_name) != properties.end()) {
        throw_runtime_error("property existed for new output: ", property_name);
    }

    add_property(property_name, property_t::type_t::string).set(channel_class_in);

    auto new_channel = make_output_channel(channel_class_in, name_in, *this);
    outputs[name_in] = new_channel;

    return *new_channel;
}

} // namespace jackalope
