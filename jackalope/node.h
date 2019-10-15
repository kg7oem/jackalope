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

#include <jackalope/channel.h>
#include <jackalope/node.forward.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

struct node_t : public baseobj_t, public lockable_t {
    pool_map_t<string_t, input_t *> inputs;
    pool_map_t<string_t, output_t *> outputs;

    virtual ~node_t();
    const string_t& get_name();
    input_t& add_input(const string_t& channel_class_in, const string_t& name_in);
    input_t& get_input(const string_t& name_in);
    output_t& add_output(const string_t& channel_class_in, const string_t& name_in);
    output_t& get_output(const string_t& name_in);
    virtual void input_ready(input_t& input_in);
};

} // namespace jackalope
