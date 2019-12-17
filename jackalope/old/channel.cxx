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

#include <algorithm>

#include <jackalope/channel.h>
#include <jackalope/exception.h>
#include <jackalope/node.h>

namespace jackalope {

static pool_map_t<string_t, input_constructor_t> input_constructors;
static pool_map_t<string_t, output_constructor_t> output_constructors;

const string_t extract_channel_class(const string_t& class_in)
{
    auto type_start_char_at = class_in.find('[');

    if (type_start_char_at == string_t::npos) {
        return class_in;
    }

    return class_in.substr(0, type_start_char_at);
}

void add_input_constructor(const string_t& class_in, input_constructor_t constructor_in)
{
    if (input_constructors.find(class_in) != input_constructors.end()) {
        throw_runtime_error("constructor already exists for channel class: ", class_in);
    }

    input_constructors[class_in] = constructor_in;
}

void add_output_constructor(const string_t& class_in, output_constructor_t constructor_in)
{
    if (output_constructors.find(class_in) != output_constructors.end()) {
        throw_runtime_error("constructor already exists for channel class: ", class_in);
    }

    output_constructors[class_in] = constructor_in;
}

shared_t<input_t> make_input_channel(const string_t& class_in, const string_t& name_in, shared_t<node_t> parent_in)
{
    auto found = input_constructors.find(class_in);

    if (found == input_constructors.end()) {
        throw_runtime_error("unknown input channel class: ", class_in);
    }

    return found->second(name_in, parent_in);
}

shared_t<output_t> make_output_channel(const string_t& class_in, const string_t& name_in, shared_t<node_t> parent_in)
{
    auto found = output_constructors.find(class_in);

    if (found == output_constructors.end()) {
        throw_runtime_error("unknown output channel class: ", class_in);
    }

    return found->second(name_in, parent_in);
}

channel_t::channel_t(const string_t& class_name_in, const string_t& name_in, shared_t<node_t> parent_in)
: name(name_in), class_name(class_name_in), parent(parent_in)
{ }

void channel_t::reset()
{ }

shared_t<node_t> channel_t::get_parent()
{
    return parent;
}

const string_t& channel_t::get_name()
{
    return name;
}

const string_t& channel_t::get_class_name()
{
    return class_name;
}

void channel_t::add_link(shared_t<link_t> link_in)
{
    links.push_back(link_in);
}

void channel_t::remove_link(shared_t<link_t> link_in)
{
    auto found = std::find(std::begin(links), std::end(links), link_in);

    if (found == std::end(links)) {
        throw_runtime_error("could not find channel link");
    }

    links.erase(found);
}

link_t::link_t(weak_t<output_t> from_in, weak_t<input_t> to_in)
: from(from_in), to(to_in)
{ }

shared_t<input_t> link_t::get_to()
{
    return to.lock();
}

shared_t<output_t> link_t::get_from()
{
    return from.lock();
}

input_t::input_t(const string_t& class_name_in, const string_t& name_in, shared_t<node_t> parent_in)
: channel_t(class_name_in, name_in, parent_in)
{ }

bool input_t::is_ready()
{
    if (links.size() == 0) {
        return true;
    }

    for(auto i : links) {
        if (! i->get_from()->is_ready()) {
            return false;
        }
    }

    return true;
}

void input_t::notify()
{
    parent->input_ready(shared_obj());
}

output_t::output_t(const string_t& class_name_in, const string_t& name_in, shared_t<node_t> parent_in)
: channel_t(class_name_in, name_in, parent_in)
{ }

void output_t::set_dirty()
{
    assert(dirty_flag == false);

    dirty_flag = true;
}

bool output_t::is_dirty()
{
    return dirty_flag;
}

bool output_t::is_ready()
{
    return is_dirty();
}

void output_t::notify()
{
    if (! parent->is_started()) {
        throw_runtime_error("output can not notify from a node that has not been started");
    }

    if (! dirty_flag) {
        throw_runtime_error("attempt to notify from an output that was not dirty: ", name);
    }

    for (auto i : links) {
        i->get_to()->output_ready(shared_obj());
    }
}

void output_t::reset()
{
    assert(dirty_flag == true);

    dirty_flag = false;

    channel_t::reset();
}

} // namespace jackalope