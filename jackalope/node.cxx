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

shared_t<node_t> node_t::make(const init_list_t& init_list_in)
{
    auto node = jackalope::make_shared<node_t>(init_list_in);
    node->init();
    return node;
}

node_t::node_t(const init_list_t& init_list_in)
: object_t(init_list_in)
{ }

} // namespace jackalope
