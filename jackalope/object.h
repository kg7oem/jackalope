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

#include <jackalope/async.h>
#include <jackalope/message.h>
#include <jackalope/property.h>
#include <jackalope/signal.h>
#include <jackalope/string.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_PROPERTY_OBJECT_TYPE "object.type"
#define JACKALOPE_SIGNAL_OBJECT_DID_ACTIVATE "object.did_activate"
#define JACKALOPE_SIGNAL_OBJECT_DID_INIT "object.did_init"
#define JACKALOPE_SIGNAL_OBJECT_DID_SHUTDOWN "object.did_shutdown"
#define JACKALOPE_SIGNAL_OBJECT_WILL_ACTTIVATE "object.will_activate"
#define JACKALOPE_SIGNAL_OBJECT_WILL_INIT "object.will_init"
#define JACKALOPE_SIGNAL_OBJECT_WILL_SHUTDOWN "object.will_shutdown"
#define JACKALOPE_SLOT_OBJECT_START "object.start"
#define JACKALOPE_SLOT_OBJECT_STOP "object.stop"

#define JACKALOPE_OBJECT_LOG_VARGS(level, ...) JACKALOPE_LOG_VARGS(JACKALOPE_LOG_NAME, jackalope::log::level_t::level, "(", this->description(), ") ", __VA_ARGS__)
#define object_log_info(...) JACKALOPE_OBJECT_LOG_VARGS(info, __VA_ARGS__)
#define object_log_trace(...) JACKALOPE_OBJECT_LOG_VARGS(trace, __VA_ARGS__)

namespace jackalope {

class object_t :
    public shared_obj_t<object_t>, public lock_obj_t, public prop_obj_t,
    public signal_obj_t, public message_obj_t {

protected:
    const init_args_t init_args;
    atomic_t<bool> initialized_flag = ATOMIC_VAR_INIT(false);
    atomic_t<bool> activated_flag = ATOMIC_VAR_INIT(false);
    atomic_t<bool> running_flag = ATOMIC_VAR_INIT(false);
    condition_t running_condition;
    atomic_t<bool> stopped_flag = ATOMIC_VAR_INIT(false);
    atomic_t<bool> shutdown_flag = ATOMIC_VAR_INIT(false);
    const shared_t<async_engine_t> async_engine = get_async_engine();

    static size_t next_object_id();
    object_t(const init_args_t& init_args_in);
    virtual shared_t<property_t> _add_property(const string_t& name_in, const property_t::type_t type_in) override;
    virtual std::pair<bool, string_t> get_property_default(const string_t& name_in);
    virtual void will_init();
    virtual void did_init();
    virtual void will_activate();
    virtual void did_activate();
    virtual void will_start();
    virtual void did_start();
    virtual void will_stop();
    virtual void did_stop();
    virtual void will_shutdown();
    virtual void did_shutdown();
    virtual void submit_job(async_job_t<void> job_in);
    virtual bool should_deliver() override;
    void deliver_one_message(shared_t<abstract_message_t> message_in) override;
    virtual void message_invoke_slot(const string_t slot_name_in);

public:
    const size_t id = next_object_id();

    virtual ~object_t();
    virtual void init();
    virtual void activate();
    virtual void start();
    virtual void stop();
    virtual void shutdown();
    virtual string_t description();
    virtual const string_t& get_type() = 0;
    virtual bool is_running();
    virtual void _send_message(shared_t<abstract_message_t> message_in);

    template <typename T, typename... Args>
    void send_message(Args... args)
    {
        auto message = jackalope::make_shared<T>(args...);
        _send_message(message);
    }

    virtual void post_slot(const string_t& name_in);
    void wait_stopped();
};

} //namespace jackalope
