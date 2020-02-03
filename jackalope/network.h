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

#include <jackalope/node.h>
#include <jackalope/types.h>

#define JACKALOPE_TYPE_NETWORK "jackalope::network"

namespace jackalope {

class network_t : public node_t {

protected:
    pool_map_t<string_t, shared_t<sink_t>> source_forward_sinks;
    pool_map_t<string_t, shared_t<source_t>> sink_forward_sources;

public:
    static shared_t<network_t> make(const init_args_t& init_args_in);
    network_t(const init_args_t& init_args_in);
    virtual shared_t<source_t> add_source(const string_t& source_name_in, const string_t& type_in) override;
    virtual shared_t<sink_t> add_sink(const string_t& sink_name_in, const string_t& type_in) override;
};

} //namespace jackalope
