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

#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class domain_t;

struct node_t : public base_t, public shared_obj_t<node_t>, public lockable_t {

public:
    void connect(const string_t& from_in, shared_t<domain_t> domain_in, const string_t& to_in);
    void link(const string_t& from_in, shared_t<node_t> node_in, const string_t& to_in);
    void link(const string_t& from_in, shared_t<domain_t> domain_in, const string_t& to_in);
};

} // namespace jackalope
