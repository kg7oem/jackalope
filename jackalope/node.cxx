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
#include <jackalope/jackalope.h>
#include <jackalope/logging.h>
#include <jackalope/node.h>

namespace jackalope {

static pool_map_t<string_t, node_constructor_t> node_constructors;

void add_node_constructor(const string_t& class_name_in, node_constructor_t constructor_in)
{
    if (node_constructors.find(class_name_in) != node_constructors.end()) {
        throw_runtime_error("Can not add duplicate node constructor for class: ", class_name_in);
    }

    node_constructors[class_name_in] = constructor_in;
}

node_t * _make_node(const string_t& node_name_in, const string_t& class_name_in, node_init_list_t init_list_in)
{
    auto found = node_constructors.find(class_name_in);

    if (found == node_constructors.end()) {
        throw_runtime_error("Could not find constructor for node class: ", class_name_in);
    }

    return found->second(node_name_in, init_list_in);
}

node_t::node_t(const string_t& name_in, const string_t& class_name_in, node_init_list_t node_init_list)
: name(name_in), class_name(class_name_in)
{
    for (auto i : node_init_list) {
        prop_args.emplace(i);
    }

    add_property(JACKALOPE_NODE_PROPERTY_NAME, property_t::type_t::string).set(name_in);
    add_property(JACKALOPE_NODE_PROPERTY_CLASS, property_t::type_t::string).set(class_name_in);
}

node_t::~node_t()
{

    inputs_by_name.empty();

    for (auto i : inputs) {
        delete i;
    }

    outputs_by_name.empty();

    for (auto i : outputs) {
        delete i;
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

void node_t::reset()
{
    for(auto i : outputs) {
        i->reset();
    }
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

    auto& property = result.first->second;

    auto prop_arg_found = prop_args.find(name_in);
    if (prop_arg_found != prop_args.end()) {
        property.set(prop_arg_found->second);
    }

    return property;
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

    if (inputs_by_name.find(channel_class_in) != inputs_by_name.end()) {
        throw_runtime_error("duplicate input name: ", name_in);
    }

    auto property_name = vaargs_to_string("input:", name_in);

    if (properties.find(property_name) != properties.end()) {
        throw_runtime_error("property existed for new input: ", property_name);
    }

    add_property(property_name, property_t::type_t::string).set(channel_class_in);

    auto new_channel = make_input_channel(channel_class_in, name_in, *this);

    inputs.push_back(new_channel);
    inputs_by_name[name_in] = new_channel;

    return *new_channel;
}

input_t& node_t::_get_input(const string_t& name_in)
{
    auto found = inputs_by_name.find(name_in);

    if (found == inputs_by_name.end()) {
        throw_runtime_error("Could not find input: ", name_in);
    }

    return *found->second;
}

const node_t::inputs_vector_t& node_t::get_inputs()
{
    return inputs;
}

output_t& node_t::_get_output(const string_t& name_in)
{
    auto found = outputs_by_name.find(name_in);

    if (found == outputs_by_name.end()) {
        throw_runtime_error("Could not find output: ", name_in);
    }

    return *found->second;
}

const node_t::outputs_vector_t& node_t::get_outputs()
{
    return outputs;
}

output_t& node_t::add_output(const string_t& channel_class_in, const string_t& name_in)
{
    if (activated_flag) {
        throw_runtime_error("Can not add an output to a node that has been activated");
    }

    if (inputs_by_name.find(channel_class_in) != inputs_by_name.end()) {
        throw_runtime_error("duplicate input name: ", name_in);
    }

    auto property_name = vaargs_to_string("output:", name_in);

    if (properties.find(property_name) != properties.end()) {
        throw_runtime_error("property existed for new output: ", property_name);
    }

    add_property(property_name, property_t::type_t::string).set(channel_class_in);

    auto new_channel = make_output_channel(channel_class_in, name_in, *this);

    outputs.push_back(new_channel);
    outputs_by_name[name_in] = new_channel;

    return *new_channel;
}

} // namespace jackalope
