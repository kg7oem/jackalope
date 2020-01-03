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

#include <jackalope/channel.h>
#include <jackalope/node.forward.h>
#include <jackalope/object.h>
#include <jackalope/types.h>

#define JACKALOPE_PROPERTY_NODE_NAME "node.name"

#define NODE_LOG(level, ...) JACKALOPE_LOG_VARGS(JACKALOPE_LOG_NAME, jackalope::log::level_t::level,  "node(", this->name, "): ", __VA_ARGS__)

namespace jackalope {

class node_t : public object_t {

protected:
    node_t(const init_list_t init_list_in);

public:
    const string_t name;

    virtual void activate();
    virtual void start() override;
    virtual void run() = 0;
    virtual void stop() override;
    virtual void deliver_one_message(shared_t<abstract_message_t> message_in) override;
    virtual void run_if_needed();
    virtual bool should_run() = 0;
    // virtual void schedule_run();
    // virtual void handle_run();
//     virtual void source_available(shared_t<source_t> source_in) override;
//     virtual void sink_ready(shared_t<sink_t> sink_in) override;
};

} //namespace jackalope
