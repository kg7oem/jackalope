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

#include <jackalope/object.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class graph_t : public object_t {

protected:
    const init_list_t init_args;
    pool_list_t<shared_t<object_t>> objects;

public:
    static shared_t<graph_t> make(const init_list_t& init_args_in);
    graph_t(const init_list_t& init_args_in);
    shared_t<object_t> add_object(const init_list_t& init_args_in);
    virtual void start() override;
};

} // namespace jackalope
