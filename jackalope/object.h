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

#include <jackalope/async.h>
#include <jackalope/channel.h>
#include <jackalope/dbus.h>
#include <jackalope/foreign.forward.h>
#include <jackalope/logging.h>
#include <jackalope/message.h>
#include <jackalope/property.h>
#include <jackalope/signal.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

#define JACKALOPE_MESSAGE_OBJECT_LINK_AVAILABLE    "object.link_available"
#define JACKALOPE_MESSAGE_OBJECT_LINK_READY        "object.link_ready"
#define JACKALOPE_PROPERTY_OBJECT_TYPE             "object.type"
#define JACKALOPE_SLOT_OBJECT_STOP                 "object.stop"
#define JACKALOPE_SIGNAL_OBJECT_STOPPED            "object.stopped"

using object_library_t = library_t<object_t, const init_args_t>;

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

struct object_dbus_t : public object_adaptor, public DBus::IntrospectableAdaptor, public DBus::ObjectAdaptor {
    object_t& object;

    object_dbus_t(object_t& object_in, const char * path_in);
    virtual std::map<std::string, std::string> get_properties() override;
    virtual std::string peek(const std::string& property_name_in) override;
    virtual void poke(const std::string& property_name_in, const std::string& value_in) override;
};

class object_t : public prop_obj_t, public signal_obj_t, public shared_obj_t<object_t>, public lockable_t, public base_t {

    friend jackalope_node_t;

protected:
    object_dbus_t * dbus_object = nullptr;

    bool init_flag = false;
    bool activated_flag = false;
    bool started_flag = false;
    bool executing_flag = false;
    bool stopped_flag = false;
    pool_map_t<string_t, shared_t<abstract_message_handler_t>> message_handlers;
    mutex_t message_mutex;
    pool_list_t<shared_t<abstract_message_t>> message_queue;
    pool_vector_t<shared_t<source_t>> sources;
    pool_map_t<string_t, shared_t<source_t>> sources_by_name;
    pool_vector_t<shared_t<sink_t>> sinks;
    pool_map_t<string_t, shared_t<sink_t>> sinks_by_name;

    object_t(const init_args_t init_args_in);
    object_t(const string_t& type_in, const init_args_t init_args_in);

    static shared_t<object_t> _make(const init_args_t init_args_in);

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
    const init_args_t init_args;
    const string_t type;
    const size_t id = _get_object_id();

    template <class T = object_t>
    static shared_t<T> make(const init_args_t init_args_in)
    {
        return dynamic_pointer_cast<T>(_make(init_args_in));
    }

    // this method is called by other threads and should not
    // lock the object mutex to avoid race conditions
    template <typename T, typename... Args>
    void send_message(Args... args)
    {
        assert(! thread_owns_mutex());
        lock_t message_lock(message_mutex);

        auto shared_this = shared_obj();
        auto message = jackalope::make_shared<T>(args...);
        message_queue.push_back(message);

        submit_job([shared_this] {
            // no problem with a lock from inside the thread queue
            auto lock = shared_this->get_object_lock();
            shared_this->check_execute();
        });
    }

    virtual void message_link_available(shared_t<link_t> link_in);
    virtual void message_link_ready(shared_t<link_t> link_in);

    virtual bool is_stopped();
    virtual string_t peek(const string_t& property_name_in);
    virtual void poke(const string_t& property_name_in, const string_t& value_in);
    virtual shared_t<source_t> add_source(const string_t& source_name_in, const string_t& type_in);
    virtual shared_t<source_t> get_source(const string_t& source_name_in);
    virtual shared_t<source_t> get_source(const size_t source_num_in);
    virtual size_t get_num_sources();
    virtual shared_t<sink_t> add_sink(const string_t& sink_name_in, const string_t& type_in);
    virtual shared_t<sink_t> get_sink(const string_t& sink_name_in);
    virtual shared_t<sink_t> get_sink(const size_t sink_num_in);
    virtual size_t get_num_sinks();
    virtual void link(const string_t& source_name_in, shared_t<object_t> target_object_in, const string_t& target_sink_name_in);
    virtual void init();
    virtual void activate();
    virtual void start();
    virtual void stop();
};

} //namespace jackalope
