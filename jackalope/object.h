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
#include <jackalope/foreign.forward.h>
#include <jackalope/logging.h>
#include <jackalope/message.h>
#include <jackalope/property.h>
#include <jackalope/signal.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

#define JACKALOPE_MESSAGE_OBJECT_LINK_AVAILABLE "object.link_available"
#define JACKALOPE_MESSAGE_OBJECT_LINK_READY "object.link_ready"
#define JACKALOPE_PROPERTY_OBJECT_TYPE "object.type"

using object_library_t = library_t<object_t, const init_list_t>;

void add_object_constructor(const string_t& class_name_in, object_library_t::constructor_t constructor_in);
size_t _get_object_id();

struct link_available_message_t : public message_t<shared_t<link_t>> {
    static const string_t message_name;
    link_available_message_t(shared_t<link_t> link_in);
};

struct link_ready_message_t : public message_t<shared_t<link_t>> {
    static const string_t message_name;
    link_ready_message_t(shared_t<link_t> link_in);
};

class object_t : public prop_obj_t, public shared_obj_t<object_t>, public lockable_t, public base_t {

    friend foreign::node_t;

protected:
    const init_args_t init_args;
    const string_t type;
    bool executing_flag = false;
    bool stopped_flag = false;
    pool_map_t<string_t, shared_t<abstract_message_handler_t>> message_handlers;
    pool_list_t<shared_t<abstract_message_t>> message_queue;
    pool_vector_t<shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<source_t>> sources_by_name;
    pool_vector_t<shared_t<sink_t>> sinks;
    pool_map_t<string_t, shared_t<sink_t>> sinks_by_name;

    object_t(const init_list_t init_list_in);
    object_t(const string_t& type_in, const init_list_t init_list_in);

    static shared_t<object_t> _make(const init_list_t init_list_in);

    template <typename T>
    void add_message_handler(typename T::handler_t handler_in)
    {
        assert_lockable_owner();

        auto found = message_handlers.find(T::message_name);

        if (found != message_handlers.end()) {
            throw_runtime_error("Attempt to add duplicate message handler: ", T::message_name);
        }

        auto message_handler = jackalope::make_shared<typename T::message_handler_t>(handler_in);
        message_handlers[T::message_name] = message_handler;
    }

    shared_t<abstract_message_handler_t> get_message_handler(const string_t& name_in);
    virtual void deliver_messages();
    virtual void deliver_one_message(shared_t<abstract_message_t> message_in);
    virtual void check_execute();

public:
    const size_t id = _get_object_id();

    template <class T = object_t>
    static shared_t<T> make(const init_list_t init_list_in)
    {
        return dynamic_pointer_cast<T>(_make(init_list_in));
    }

    template <typename T, typename... Args>
    void send_message(Args... args)
    {
        auto shared_this = shared_obj();

        // FIXME until the message queue is lockfree
        // getting the message delivery running from
        // the thread queue prevents deadlocks
        submit_job([shared_this, args...] {
            log_info("getting object lock");
            auto lock = shared_this->get_object_lock();
            log_info("done getting object lock");

            auto message = jackalope::make_shared<T>(args...);
            shared_this->message_queue.push_back(message);

            shared_this->check_execute();
        });
    }

    virtual void message_link_available(shared_t<link_t> link_in);
    virtual void message_link_ready(shared_t<link_t> link_in);

    virtual shared_t<source_t> add_source(const string_t& source_name_in, const string_t& type_in);
    virtual shared_t<source_t> get_source(const string_t& source_name_in);
    virtual shared_t<source_t> get_source(const size_t source_num_in);
    virtual shared_t<sink_t> add_sink(const string_t& sink_name_in, const string_t& type_in);
    virtual shared_t<sink_t> get_sink(const string_t& sink_name_in);
    virtual shared_t<sink_t> get_sink(const size_t sink_num_in);
    virtual void link(const string_t& source_name_in, shared_t<object_t> target_object_in, const string_t& target_sink_name_in);
    virtual void init();
    virtual void activate();
    virtual void start();
    virtual void stop();
};

} //namespace jackalope
