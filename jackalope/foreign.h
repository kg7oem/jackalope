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

namespace jackalope {

namespace foreign {

struct node_t {

protected:
    const shared_t<jackalope::node_t> node;

public:
    node_t(shared_t<jackalope::node_t> node_in);
};

template <typename... T>
node_t make_node(T... args)
{
    auto new_node = jackalope::object_t::make<jackalope::node_t>(args...);
    return node_t(new_node);
}

} // namespace foreign

} //namespace jackalope
