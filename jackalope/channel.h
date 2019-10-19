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

#pragma once

#include <jackalope/node.forward.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

struct input_t;
struct output_t;
struct link_t;

using input_constructor_t = function_t<input_t * (const string_t& name_in, shared_t<node_t> parent_in)>;
using output_constructor_t = function_t<output_t * (const string_t& name_in, shared_t<node_t> parent_in)>;

struct channel_t : public baseobj_t {
    const string_t name;
    const string_t class_name;
    shared_t<node_t> parent;
    pool_list_t<link_t *> links;

    channel_t(const string_t& class_name_in, const string_t& name_in, shared_t<node_t> parent_in);
    virtual ~channel_t() = default;
    virtual shared_t<node_t> get_parent();
    virtual const string_t& get_name();
    virtual const string_t& get_class_name();
    virtual void add_link(link_t * link_in);
    virtual void remove_link(link_t * link_in);
    virtual void unlink(link_t * link_in) = 0;
    virtual bool is_ready() = 0;
    virtual void reset();
};

struct link_t : public baseobj_t {
    output_t& from;
    input_t& to;

    link_t(output_t& from_in, input_t& to_in);
    virtual ~link_t() = default;
};

struct input_t : public channel_t {
    input_t(const string_t& class_name_in, const string_t& name_in, shared_t<node_t> parent_in);
    virtual ~input_t() = default;
    virtual bool is_ready();
    virtual void link(output_t& output_in) = 0;
    virtual void notify();
    virtual void output_ready(output_t& output_in) = 0;
};

struct output_t : public channel_t {
    bool dirty_flag = false;

    output_t(const string_t& class_name_in, const string_t& name_in, shared_t<node_t> parent_in);
    virtual ~output_t() = default;
    virtual void set_dirty();
    virtual bool is_dirty();
    virtual bool is_ready() override;
    virtual void notify();
    virtual void link(input_t& input_in) = 0;
    virtual void reset() override;
};

// example channel classes and classes with types
//   midi
//   pcm[real]
//   pcm[quad]
//   bitmap[rgb]
//   bitmap[cmyk]
const string_t extract_channel_class(const string_t& class_in);
const string_t extract_channel_type(const string_t& class_in);

void add_input_constructor(const string_t& class_in, input_constructor_t constructor_in);
void add_output_constructor(const string_t& class_in, output_constructor_t constructor_in);
input_t * make_input_channel(const string_t& class_in, const string_t& name_in, shared_t<node_t> parent_in);
output_t * make_output_channel(const string_t& class_in, const string_t& name_in, shared_t<node_t> parent_in);

} // namespace jackalope
