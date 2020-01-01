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

#include <jackalope/node.h>

namespace jackalope {

node_t::node_t(const init_list_t init_list_in)
: object_t(init_list_in), type(init_args_get(JACKALOPE_PROPERTY_NODE_TYPE, init_args)), name(init_args_get(JACKALOPE_PROPERTY_NODE_NAME, init_args))
{
    assert(type != "");
    assert(name != "");
}

} //namespace jackalope
