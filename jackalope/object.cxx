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

#include <jackalope/channel.h>
#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/object.h>

namespace jackalope {

static const pool_vector_t<string_t> object_signal_names = {
     JACKALOPE_SIGNAL_OBJECT_DID_ACTIVATE, JACKALOPE_SIGNAL_OBJECT_DID_INIT,
     JACKALOPE_SIGNAL_OBJECT_DID_START, JACKALOPE_SIGNAL_OBJECT_DID_STOP
};

static const pool_vector_t<std::pair<string_t, void (object_t::*)()>> object_slot_handlers = {
    { JACKALOPE_SLOT_OBJECT_START, &object_t::start },
    { JACKALOPE_SLOT_OBJECT_STOP, &object_t::stop },
};

size_t object_t::next_object_id()
{
    static atomic_t<size_t> current_id = ATOMIC_VAR_INIT(0);
    return ++current_id;
}

object_t::object_t(const init_args_t& init_args_in)
: init_args(init_args_in)
{ }

object_t::~object_t()
{
    assert(shutdown_flag);
}

void object_t::init()
{
    assert_lockable_owner();

    if (initialized_flag) {
        throw_runtime_error("Can't call init() on a node that has already been initialized: ", description());
    }

    object_log_trace("initializing object");

    will_init();
    initialized_flag = true;
    did_init();

    send_signal(JACKALOPE_SIGNAL_OBJECT_DID_INIT);

    object_log_trace("Done initializing object");
}

void object_t::will_init()
{
    assert_lockable_owner();
    assert(initialized_flag == false);

    add_property(JACKALOPE_PROPERTY_OBJECT_TYPE, property_t::type_t::string, get_type());

    add_message_handler<invoke_slot_message_t>([this] (const string_t& slot_name_in) { message_invoke_slot(slot_name_in); });
    add_message_handler<link_available_message_t>([this] (shared_t<link_t> link_in) { message_link_available(link_in); });
    add_message_handler<link_ready_message_t>([this] (shared_t<link_t> link_in) { message_link_ready(link_in); });

    for(auto& i : object_signal_names) {
        add_signal(i);
    }

    for(auto& i : object_slot_handlers) {
        add_slot(i.first, std::bind(i.second, this));
    }
}

void object_t::did_init()
{
    assert_lockable_owner();
    assert(initialized_flag);
}

void object_t::activate()
{
    assert_lockable_owner();

    if (! initialized_flag) {
        throw_runtime_error("Can't call activate() on a node that has not been initialized: ", description());
    }

    if (activated_flag) {
        throw_runtime_error("Can't call activate() on a node that is already activated");
    }

    log_trace("activating object");

    will_activate();
    activated_flag = true;
    did_activate();

    send_signal(JACKALOPE_SIGNAL_OBJECT_DID_ACTIVATE);

    log_trace("done activating object");
}

void object_t::will_activate()
{
    assert_lockable_owner();
    assert(activated_flag == false);
}

void object_t::did_activate()
{
    assert_lockable_owner();
    assert(activated_flag);
}

void object_t::start()
{
    assert_lockable_owner();

    if (! activated_flag) {
        throw_runtime_error("can't start an object that is not activated");
    }

    if (running_flag) {
        throw_runtime_error("can't start an object that is already running");
    }

    object_log_info("starting object");

    will_start();
    running_flag = true;
    running_condition.notify_all();
    did_start();

    send_signal(JACKALOPE_SIGNAL_OBJECT_DID_START);
}

void object_t::will_start()
{
    assert_lockable_owner();
    assert(running_flag == false);
}

void object_t::did_start()
{
    assert_lockable_owner();
    assert(running_flag == true);
}

void object_t::stop()
{
    assert_lockable_owner();

    if (! running_flag) {
        return;
    }

    object_log_info("stopping object");

    will_stop();
    running_flag = false;
    running_condition.notify_all();
    did_stop();

    send_signal(JACKALOPE_SIGNAL_OBJECT_DID_STOP);
}

void object_t::will_stop()
{
    assert_lockable_owner();
    assert(running_flag == true);
}

void object_t::did_stop()
{
    assert_lockable_owner();
    assert(running_flag == false);
}

void object_t::shutdown()
{
    assert_lockable_owner();

    if (shutdown_flag) {
        return;
    }

    if (! stopped_flag) {
        stop();
    }

    object_log_info("Shutting down object");

    will_shutdown();
    shutdown_flag = true;
    did_shutdown();
}

void object_t::will_shutdown()
{
    assert_lockable_owner();
    assert(shutdown_flag == false);
}

void object_t::did_shutdown()
{
    assert_lockable_owner();
    assert(shutdown_flag == true);

    for(auto i : signals) {
        i.second->shutdown();
    }
}

// description() must never require a lock
// even from a subclass
string_t object_t::description()
{
    return to_string("object #", id, " ", get_type());
}

bool object_t::is_running()
{
    assert_lockable_owner();

    return running_flag;
}

shared_t<property_t> object_t::_add_property(const string_t& name_in, const property_t::type_t type_in)
{
    assert_lockable_owner();
    assert_mutex_owner(property_mutex);

    auto property_default = get_property_default(name_in);
    auto property = prop_obj_t::_add_property(name_in, type_in);

    if (property_default.first) {
        property->set(property_default.second);
    }

    return property;
}

std::pair<bool, string_t> object_t::get_property_default(const string_t& name_in)
{
    assert_lockable_owner();

    if (init_args_has(name_in, init_args)) {
        return { true, init_args_get(name_in, init_args) };
    }

    return { false, "" };
}

void object_t::invoke_slot(const string_t& name_in)
{
    auto slot = get_slot(name_in);
    slot->invoke();
}

void object_t::add_channel_type(const string_t& type_in)
{
    assert_lockable_owner();

    object_log_trace("Adding channel type: ", type_in);

    auto properties = get_channel_properties(type_in);
    add_properties(properties);
}

shared_t<source_t> object_t::add_source(const string_t& type_in, const string_t& name_in)
{
    assert_lockable_owner();

    object_log_trace("Adding source: ", name_in, " = ", type_in);

    if (sources_by_name.count(name_in) != 0) {
        throw_runtime_error("can't add duplicate source: ", name_in);
    }

    auto source = source_t::make(name_in, type_in, shared_obj());
    sources_by_name[name_in] = source;
    sources.push_back(source);

    return source;
}

shared_t<source_t> object_t::get_source(const string_t& name_in)
{
    assert_lockable_owner();

    if (sources_by_name.count(name_in) == 0) {
        throw_runtime_error("can't find source: ", name_in);
    }

    return sources_by_name[name_in];
}

shared_t<source_t> object_t::get_source(const size_t num_in)
{
    assert_lockable_owner();

    if (num_in > sources.size()) {
        throw_runtime_error("invalid source number: ", num_in);
    }

    return sources[num_in];
}

void object_t::source_available(shared_t<source_t> source_in)
{
    assert_lockable_owner();

    object_log_trace("source available: ", source_in->name);
}

void object_t::sink_ready(shared_t<sink_t> sink_in)
{
    assert_lockable_owner();

    object_log_trace("sink ready: ", sink_in->name);
}

void object_t::link(const string_t& source_name_in, shared_t<object_t> target_in, const string_t& sink_name_in)
{
    assert_lockable_owner();

    auto source = get_source(source_name_in);
    auto sink = target_in->get_sink(sink_name_in);

    source->link(sink);
}

shared_t<sink_t> object_t::add_sink(const string_t& type_in, const string_t& name_in)
{
    assert_lockable_owner();

    object_log_trace("Adding sink: ", name_in, " = ", type_in);

    if (sinks_by_name.count(name_in)) {
        throw_runtime_error("can't add duplicate sink: ", name_in);
    }

    auto sink = sink_t::make(name_in, type_in, shared_obj());
    sinks_by_name[name_in] = sink;
    sinks.push_back(sink);

    return sink;
};

shared_t<sink_t> object_t::get_sink(const string_t& name_in)
{
    assert_lockable_owner();

    if (sinks_by_name.count(name_in) == 0) {
        throw_runtime_error("can't add duplicate sink: ", name_in);
    }

    return sinks_by_name[name_in];
}

shared_t<sink_t> object_t::get_sink(const size_t num_in)
{
    assert_lockable_owner();

    if (num_in > sinks.size()) {
        throw_runtime_error("invalid sink number: ", num_in);
    }

    return sinks[num_in];
}

// locking is not needed because the async_engine is const
void object_t::submit_job(async_job_t<void> job_in)
{
    // make sure a shared pointer is held inside the thread queue
    auto shared_this = shared_obj();
    async_engine->submit_job([shared_this, job_in] { job_in(); });
}

// this method has special locking requirements
void object_t::_send_message(shared_t<abstract_message_t> message_in)
{
    lock_t message_lock(message_mutex);

    message_queue.push_back(message_in);

    if (! message_delivering_flag) {
        message_delivering_flag = true;

        submit_job([this] {
            deliver_messages();
        });
    }
}

bool object_t::should_deliver()
{
    auto lock = get_object_lock();

    if (! initialized_flag) {
        return false;
    }

    if (stopped_flag) {
        return false;
    }

    return true;
}

void object_t::deliver_one_message(shared_t<abstract_message_t> message_in)
{
    guard_lockable({
        message_obj_t::deliver_one_message(message_in);
    });
}

void object_t::message_invoke_slot(const string_t slot_name_in)
{
    assert_lockable_owner();
    return invoke_slot(slot_name_in);
}

void object_t::message_link_ready(shared_t<link_t> link_in)
{
    if (! link_in->is_ready()) {
        return;
    }

    sink_ready(link_in->get_to());
}

void object_t::message_link_available(shared_t<link_t> link_in)
{
    if (! link_in->is_available()) {
        return;
    }

    source_available(link_in->get_from());
}

void object_t::post_slot(const string_t& name_in)
{
    send_message<invoke_slot_message_t>(name_in);
}

void object_t::wait_stopped()
{
    assert_lockable_owner();

    running_condition.wait(object_mutex, [&] { return running_flag == false; });
}

} //namespace jackalope
