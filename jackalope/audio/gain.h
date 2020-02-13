// Jackalope Audio Engine
// Copyright 2020 Tyler Riddle <kg7oem@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

#pragma once

#include <jackalope/plugin.h>
#include <jackalope/project.h>
#include <jackalope/types.h>

namespace jackalope {

class audio_gain_plugin_t : public filter_plugin_t {

public:
    static const string_t type;

    static shared_t<audio_gain_plugin_t> make(shared_t<project_t> project_in, const init_args_t& init_args_in);
    audio_gain_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in);
    virtual const string_t& get_type() override;
};

} //namespace jackalope
