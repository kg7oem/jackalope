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

const string_t link_available_message_t::message_name = JACKALOPE_MESSAGE_OBJECT_LINK_AVAILABLE;

link_available_message_t::link_available_message_t(shared_t<link_t> link_in)
: message_t(JACKALOPE_MESSAGE_OBJECT_LINK_AVAILABLE, link_in)
{
    assert(link_in != nullptr);
}

const string_t link_ready_message_t::message_name = JACKALOPE_MESSAGE_OBJECT_LINK_READY;

link_ready_message_t::link_ready_message_t(shared_t<link_t> link_in)
: message_t(JACKALOPE_MESSAGE_OBJECT_LINK_READY, link_in)
{
    assert(link_in != nullptr);
}

shared_t<object_t> object_t::_make(const init_args_t init_args_in)
{
    if (! init_args_has(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args_in)) {
        throw_runtime_error("missing object type");
    }

    auto object_type = init_args_get(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args_in);
    auto constructor = object_library->get_constructor(object_type);
    auto new_object = constructor(init_args_in);
    auto lock = new_object->get_object_lock();
    new_object->init();
    return new_object;
}

object_t::object_t(const init_args_t init_args_in)
: init_args(init_args_in), type(init_args_get(JACKALOPE_PROPERTY_OBJECT_TYPE, init_args))
{
    assert(type != "");
}

object_t::object_t(const string_t& type_in, const init_args_t init_args_in)
: init_args(init_args_in), type(type_in)
{
    assert(type != "");
}

object_t::~object_t()
{
    if (! stopped_flag) {
        auto lock = get_object_lock();
        stop();
    }
}

shared_t<abstract_message_handler_t> object_t::get_message_handler(const string_t& name_in)
{
    assert_lockable_owner();

    auto found = message_handlers.find(name_in);

    if (found == message_handlers.end()) {
        throw_runtime_error("could not find message handler: ", name_in);
    }

    return found->second;
}

void object_t::execute_if_needed()
{
    assert_lockable_owner();

    if (executing_flag) {
        return;
    }

    executing_flag = true;

    deliver_messages();
}

void object_t::deliver_messages()
{
    assert_lockable_owner();
    assert(executing_flag == true);

    while(1) {
        shared_t<abstract_message_t> message;

        {
            lock_t message_lock_t(message_mutex);

            if (message_queue.empty()) {
                executing_flag = false;
                return;
            }

            message = message_queue.front();
            message_queue.pop_front();
        }

        deliver_one_message(message);
    }
}

void object_t::deliver_one_message(shared_t<abstract_message_t> message_in)
{
    assert_lockable_owner();

    auto message_name = message_in->name;
    auto message_handler = get_message_handler(message_name);
    message_handler->invoke(message_in);
}

void object_t::message_link_available(shared_t<link_t> link_in) {
    assert_lockable_owner();

    auto source = link_in->get_from();
    assert(source->get_parent() == shared_obj());

    if (! link_in->is_available()) {
        return;
    }

    source->link_available(link_in);
}

void object_t::message_link_ready(shared_t<link_t> link_in)
{
    assert_lockable_owner();

    auto sink = link_in->get_to();
    assert(sink->get_parent() == shared_obj());

    if (! link_in->is_ready()) {
        return;
    }

    sink->link_ready(link_in);
}

bool object_t::is_stopped()
{
    assert_lockable_owner();

    return stopped_flag;
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

shared_t<source_t> object_t::add_source(const string_t& source_name_in, const string_t& type_in)
{
    assert_lockable_owner();

    auto found = sources_by_name.find(source_name_in);

    if (found != sources_by_name.end()) {
        throw_runtime_error("Can not add duplicate source name: ", source_name_in);
    }

    auto new_source = source_t::make(source_name_in, type_in, shared_obj());
    sources.push_back(new_source);
    sources_by_name[new_source->name] = new_source;

    return new_source;
}

shared_t<source_t> object_t::get_source(const string_t& source_name_in)
{
    assert_lockable_owner();

    auto found = sources_by_name.find(source_name_in);

    if (found == sources_by_name.end()) {
        throw_runtime_error("Unknown source name: ", source_name_in);
    }

    return found->second;
}

shared_t<source_t> object_t::get_source(const size_t source_num_in)
{
    assert_lockable_owner();

    if (source_num_in >= sources.size()) {
        throw_runtime_error("Source number is out of bounds: ", source_num_in);
    }

    return sources[source_num_in];
}

size_t object_t::get_num_sources()
{
    assert_lockable_owner();

    return sources.size();
}

shared_t<sink_t> object_t::add_sink(const string_t& sink_name_in, const string_t& type_in)
{
    assert_lockable_owner();

    auto found = sinks_by_name.find(sink_name_in);

    if (found != sinks_by_name.end()) {
        throw_runtime_error("Can not add duplicate sink name: ", sink_name_in);
    }

    auto new_source = sink_t::make(sink_name_in, type_in, shared_obj());
    sinks.push_back(new_source);
    sinks_by_name[new_source->name] = new_source;

    return new_source;
}

shared_t<sink_t> object_t::get_sink(const string_t& sink_name_in)
{
    assert_lockable_owner();

    auto found = sinks_by_name.find(sink_name_in);

    if (found == sinks_by_name.end()) {
        throw_runtime_error("Unknown sink name: ", sink_name_in);
    }

    return found->second;
}

shared_t<sink_t> object_t::get_sink(const size_t sink_num_in)
{
    assert_lockable_owner();

    if (sink_num_in >= sinks.size()) {
        throw_runtime_error("Sink number is out of bounds: ", sink_num_in);
    }

    return sinks[sink_num_in];
}

size_t object_t::get_num_sinks()
{
    assert_lockable_owner();

    return sinks.size();
}

void object_t::link(const string_t& source_name_in, shared_t<object_t> target_object_in, const string_t& target_sink_name_in)
{
    assert_lockable_owner();

    auto target_sink = target_object_in->get_sink(target_sink_name_in);
    auto source = get_source(source_name_in);

    source->link(target_sink);
}

void object_t::connect(const string_t& signal_name_in, shared_t<object_t> target_object_in, const string_t& target_slot_name_in)
{
    assert_lockable_owner();
    assert_object_owner(target_object_in);

    auto signal = get_signal(signal_name_in);
    auto slot = target_object_in->get_slot(target_slot_name_in);
    weak_t<object_t> weak_this = shared_obj();

    signal->subscribe([weak_this, slot] {
        try {
            weak_this.lock()->async_engine->submit_job([slot] {
                slot->handler();
            });
        } catch (std::bad_weak_ptr e) {

        }
    });
}

void object_t::init()
{
    assert_lockable_owner();

    assert(init_flag == false);

    init_flag = true;

    add_property(JACKALOPE_PROPERTY_OBJECT_TYPE, property_t::type_t::string, init_args);

    add_message_handler<link_ready_message_t>([this] (shared_t<link_t> link_in) { this->message_link_ready(link_in); });
    add_message_handler<link_available_message_t>([this] (shared_t<link_t> link_in) { this->message_link_available(link_in); });

    add_slot(JACKALOPE_SLOT_OBJECT_STOP, [this] () {
        auto lock = get_object_lock();
        this->stop();
    });

    add_signal(JACKALOPE_SIGNAL_OBJECT_STOPPED);

#ifdef CONFIG_HAVE_DBUS
    dbus_object = new object_dbus_t(*this, to_string("/Object/", id).c_str());
#endif
}

void object_t::activate()
{
    assert_lockable_owner();

    assert(init_flag);
    assert(! activated_flag);

    activated_flag = true;
}

void object_t::start()
{
    assert_lockable_owner();

    assert(activated_flag);
    assert(started_flag == false);

    started_flag = true;

    for(auto i : sources) {
        i->start();
    }

    for(auto i : sinks) {
        i->start();
    }
}

void object_t::stop()
{
    assert_lockable_owner();

    assert(started_flag);
    assert(stopped_flag == false);

    stopped_flag = true;

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
