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

#include <jackalope/async.h>
#include <jackalope/exception.h>
#include <jackalope/jackalope.h>
#include <jackalope/logging.h>
#include <jackalope/object.h>

namespace jackalope {

static object_library_t * object_library = new object_library_t();

void add_object_constructor(const string_t& class_name_in, object_library_t::constructor_t constructor_in)
{
    object_library->add_constructor(class_name_in, constructor_in);
}

size_t _get_object_id()
{
    static atomic_t<size_t> current_id = ATOMIC_VAR_INIT(0);

    return current_id++;
}

const string_t invoke_slot_message_t::message_name = JACKALOPE_MESSAGE_OBJECT_INVOKE_SLOT;

invoke_slot_message_t::invoke_slot_message_t(const string_t& slot_name_in)
: message_t(JACKALOPE_MESSAGE_OBJECT_INVOKE_SLOT, slot_name_in)
{
    assert(slot_name_in != "");
}

shared_t<object_t> object_t::_make(const string_t& type_in, const init_args_t& init_args_in)
{
    auto constructor = object_library->get_constructor(type_in);
    auto new_object = constructor(type_in, init_args_in);
    auto lock = new_object->get_object_lock();
    new_object->init();
    return new_object;
}

shared_t<object_t> object_t::_make(const init_args_t& init_args_in)
{
    if (! init_args_has(JACKALOPE_PROPERTY_OBJECT_TYPE, &init_args_in)) {
        throw_runtime_error("missing object type");
    }

    auto object_type = init_args_get(JACKALOPE_PROPERTY_OBJECT_TYPE, &init_args_in);
    return _make(object_type, init_args_in);
}

object_t::object_t(const string_t& type_in, const init_args_t& init_args_in)
: init_args(new init_args_t(init_args_in)), type(type_in)
{
    assert(type != "");

    own_init_args = true;

    if (init_args_has(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args)) {
        if (type_in != init_args_get(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args)) {
            throw_runtime_error(JACKALOPE_PROPERTY_OBJECT_TYPE, " in init_args did not match type argument during construction");
        }
    }
}

object_t::object_t(const string_t& type_in, const init_args_t * init_args_in)
: init_args(init_args_in), type(type_in)
{
    assert(type != "");
    assert(init_args != nullptr);

    if (init_args_has(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args)) {
        if (type_in != init_args_get(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args)) {
            throw_runtime_error("object type in init_args did not match object type during construction");
        }
    }
}

object_t::~object_t()
{
    if (! stopped_flag) {
        auto lock = get_object_lock();
        stop();
    }

    if (own_init_args) {
        delete init_args;
    }
}

string_t object_t::description()
{
    return to_string(type, " #", id);
}

void object_t::init()
{
    assert_lockable_owner();

    assert(init_flag == false);

    init_flag = true;

    add_property(JACKALOPE_PROPERTY_OBJECT_TYPE, property_t::type_t::string, init_args);

    add_message_handler<invoke_slot_message_t>([this] (const string_t& slot_name_in) { this->message_invoke_slot(slot_name_in); });

    add_slot(JACKALOPE_SLOT_OBJECT_STOP, std::bind(&object_t::stop, this));

    add_signal(JACKALOPE_SIGNAL_OBJECT_STOPPED);

#ifdef CONFIG_HAVE_DBUS
    dbus_object = new object_dbus_t(*this, to_string("/Object/", id).c_str());
#endif
}

bool object_t::is_stopped()
{
    assert_lockable_owner();

    return stopped_flag;
}

// this method has special locking requirements
void object_t::_send_message(shared_t<abstract_message_t> message_in)
{
    lock_t message_lock(message_mutex);

    message_queue.push_back(message_in);

    if (! message_delivering_flag) {
        auto shared_this = shared_obj();

        message_delivering_flag = true;

        async_engine->submit_job([shared_this] {
            shared_this->deliver_messages();
        });
    }
}

bool object_t::should_deliver()
{
    auto lock = get_object_lock();

    if (stopped_flag) {
        return false;
    }

    return true;
}

void object_t::deliver_one_message(shared_t<abstract_message_t> message_in)
{
    auto lock = get_object_lock();
    message_obj_t::deliver_one_message(message_in);
}

string_t object_t::peek(const string_t& property_name_in)
{
    assert_lockable_owner();

    return get_property(property_name_in)->get();
}

void object_t::poke(const string_t& property_name_in, const string_t& value_in)
{
    assert_lockable_owner();

    get_property(property_name_in)->set(value_in);
}

void object_t::subscribe(const string_t& signal_name_in, shared_t<object_t> target_object_in, const string_t& target_slot_name_in)
{
    assert_lockable_owner();
    assert_object_owner(target_object_in);

    auto signal = get_signal(signal_name_in);
    signal->subscribe(target_object_in, target_slot_name_in);
}

void object_t::message_invoke_slot(const string_t slot_name_in)
{
    assert_lockable_owner();

    auto slot = get_slot(slot_name_in);
    slot->invoke();
}

void object_t::start()
{
    assert_lockable_owner();

    if (! init_flag) {
        throw_runtime_error("attempt to start an object that was not initialized");
    }

    if (started_flag) {
        throw_runtime_error("attempt to start an object that has already been started");
    }

    started_flag = true;
}

void object_t::stop()
{
    assert_lockable_owner();

    if (stopped_flag) {
        throw_runtime_error("Can't stop a node that is already stopped");
    }

    stopped_flag = true;

    message_queue.empty();

    get_signal(JACKALOPE_SIGNAL_OBJECT_STOPPED)->send();
}

#ifdef CONFIG_HAVE_DBUS
object_dbus_t::object_dbus_t(object_t& object_in, const char * path_in)
: DBus::ObjectAdaptor(dbus_get_connection(), path_in), object(object_in)
{ }

std::map<std::string, std::string> object_dbus_t::get_properties()
{
    std::map<std::string, std::string> retval;

    auto properties = wait_job<const pool_map_t<string_t, shared_t<property_t>>>([&] {
        auto lock = object.get_object_lock();
        return object.get_properties();
    });

    for(auto i : properties) {
        retval.emplace(i.first.c_str(), i.second->get().c_str());
    }

    return retval;
}

std::string object_dbus_t::peek(const std::string& property_name_in)
{
    auto result = wait_job<string_t>([&] {
        auto lock = object.get_object_lock();
        return object.get_property(property_name_in.c_str())->get_string();
    });

    return std::string(result.c_str());
}

void object_dbus_t::poke(const std::string& property_name_in, const std::string& value_in)
{
    wait_job<void>([&] {
        auto lock = object.get_object_lock();
        object.get_property(property_name_in.c_str())->set(value_in.c_str());
    });
}
#endif

} //namespace jackalope
