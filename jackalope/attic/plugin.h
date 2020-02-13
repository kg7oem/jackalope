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

class plugin_t : public node_t {

protected:
    plugin_t(const init_args_t init_args_in);
    virtual bool should_execute() = 0;
    virtual void execute_if_needed();
    virtual void execute() = 0;
    virtual void message_invoke_slot(const string_t slot_name_in) override;
    virtual void sink_ready(shared_t<sink_t> sink_in) override;
    virtual void source_available(shared_t<source_t> source_in) override;

public:
    virtual void start() override;
};

class driver_t : public plugin_t {

protected:
    driver_t(const init_args_t init_args_in);
    bool should_execute() override;
};

class threaded_driver_t : public driver_t {

protected:
    condition_t driver_thread_cond;
    bool driver_thread_run_flag = false;

    threaded_driver_t(const init_args_t init_args_in);
    bool should_execute() override;
    void execute() override;
    void stop() override;
};

class filter_plugin_t : public plugin_t {

protected:
    filter_plugin_t(const init_args_t init_args_in);
    virtual bool should_execute() override;
};

} //namespace jackalope
