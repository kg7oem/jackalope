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
#include <jackalope/foreign.forward.h>
#include <jackalope/logging.h>
#include <jackalope/message.h>
#include <jackalope/property.h>
#include <jackalope/signal.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#ifdef CONFIG_HAVE_DBUS
#include <jackalope/dbus.h>
#endif

namespace jackalope {

#define JACKALOPE_MESSAGE_OBJECT_INVOKE_SLOT       "object.invoke_slot"
#define JACKALOPE_PROPERTY_OBJECT_TYPE             "object.type"
#define JACKALOPE_PROPERTY_OBJECT_async_engine     "object.async_engine"
#define JACKALOPE_SLOT_OBJECT_STOP                 "object.stop"
#define JACKALOPE_SIGNAL_OBJECT_STOPPED            "object.stopped"

#define JACKALOPE_OBJECT_LOG_VARGS(level, ...) JACKALOPE_LOG_VARGS(JACKALOPE_LOG_NAME, jackalope::log::level_t::level, "(", this->description(), ") ", __VA_ARGS__)
#define object_log_info(...) JACKALOPE_OBJECT_LOG_VARGS(info, __VA_ARGS__)

using object_library_t = library_t<object_t, const string_t&, const init_args_t&>;

void add_object_constructor(const string_t& class_name_in, object_library_t::constructor_t constructor_in);
size_t _get_object_id();

struct invoke_slot_message_t : public message_t<const string_t> {
    static const string_t message_name;
    invoke_slot_message_t(const string_t& slot_name_in);
};

#ifdef CONFIG_HAVE_DBUS
struct object_dbus_t : public object_adaptor, public DBus::IntrospectableAdaptor, public DBus::ObjectAdaptor {
    object_t& object;

    object_dbus_t(object_t& object_in, const char * path_in);
    virtual std::map<std::string, std::string> get_properties() override;
    virtual std::string peek(const std::string& property_name_in) override;
    virtual void poke(const std::string& property_name_in, const std::string& value_in) override;
};
#endif

class object_t :
    public prop_obj_t, public signal_obj_t, public message_obj_t,
    public shared_obj_t<object_t>, public lockable_t, public base_t {

    friend jackalope_node_t;
    friend jackalope_object_t;

protected:
#ifdef CONFIG_HAVE_DBUS
    object_dbus_t * dbus_object = nullptr;
#endif

    bool init_flag = false;
    bool started_flag = false;
    bool stopped_flag = false;
    bool own_init_args = false;
    const shared_t<async_engine_t> async_engine = get_async_engine();

    object_t(const string_t& type_in, const init_args_t& init_args_in);
    object_t(const string_t& type_in, const init_args_t * init_args_in);
    virtual ~object_t();

    static shared_t<object_t> _make(const string_t& type_in, const init_args_t& init_args_in);
    static shared_t<object_t> _make(const init_args_t& init_args_in);

    virtual bool should_deliver() override;
    virtual void message_invoke_slot(const string_t slot_name_in);

public:
    const init_args_t * init_args = nullptr;
    const string_t type;
    const size_t id = _get_object_id();

    template <class T = object_t, typename... Args>
    static shared_t<T> make(Args... args_in)
    {
        return dynamic_pointer_cast<T>(_make(args_in...));
    }

    virtual string_t description();

    virtual void _send_message(shared_t<abstract_message_t> message_in);
    virtual void deliver_one_message(shared_t<abstract_message_t> message_in) override;

    template <typename T, typename... Args>
    void send_message(Args... args)
    {
        auto message = jackalope::make_shared<T>(args...);
        _send_message(message);
    }

    virtual void connect(const string_t& signal_name_in, shared_t<object_t> target_object_in, const string_t& target_slot_name_in);

    virtual bool is_stopped();
    virtual string_t peek(const string_t& property_name_in);
    virtual void poke(const string_t& property_name_in, const string_t& value_in);
    virtual void init();
    virtual void start();
    virtual void stop();
};

} //namespace jackalope
