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

#include <jackalope/forward.h>
#include <jackalope/node.h>
#include <jackalope/types.h>

namespace jackalope {

using plugin_constructor_t = function_t<shared_t<plugin_t> (shared_t<project_t>, const init_args_t&)>;

void add_plugin_constructor(const string_t& type_in, plugin_constructor_t constructor_in);
plugin_constructor_t get_plugin_constructor(const string_t& type_in);

class plugin_t : public node_t {

protected:
    plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in);
    virtual bool should_execute();
    virtual void execute_if_needed();
    virtual void execute() = 0;
    virtual void invoke_slot(const string_t& name_in) override;
    virtual void sink_ready(shared_t<sink_t> sink_in) override;
    virtual bool sinks_are_ready();
    virtual void source_available(shared_t<source_t> source_in) override;
    virtual bool sources_are_available();
};

// a driver plugin executes when all sources are available
class driver_plugin_t : public plugin_t {

protected:
    driver_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in);
    virtual bool should_execute() override;
};

// a filter plugin executes when all sinks are ready all sources are available
class filter_plugin_t : public plugin_t {

protected:
    filter_plugin_t(shared_t<project_t> project_in, const init_args_t& init_args_in);
    virtual bool should_execute() override;
};

} //namespace jackalope
