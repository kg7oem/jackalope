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

#include <jackalope/foreign.forward.h>
#include <jackalope/node.h>
#include <jackalope/types.h>

namespace jackalope {

namespace foreign {

template <typename T>
class wrapper_t {

protected:
    const shared_t<T> wrapped;

    wrapper_t(shared_t<T> wrapped_in)
    : wrapped(wrapped_in)
    {
        assert(wrapped_in != nullptr);
    }
};

struct source_t : public wrapper_t<jackalope::source_t> {
    source_t(shared_t<jackalope::source_t> wrapped_in);
};

struct sink_t : public wrapper_t<jackalope::sink_t> {
    sink_t(shared_t<jackalope::sink_t> wrapped_in);
};

struct node_t : public wrapper_t<jackalope::node_t> {
    node_t(shared_t<jackalope::node_t> wrapped_in);
    source_t add_source(const string_t& name_in, const string_t& type_in);
    sink_t add_sink(const string_t& name_in, const string_t& type_in);
    void link(const string_t& source_name_in, node_t target_object_in, const string_t& target_sink_name_in);
    void activate();
    void start();
};

node_t make_node(const init_list_t init_list_in);

} // namespace foreign

} //namespace jackalope
