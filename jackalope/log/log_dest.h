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

#include <jackalope/log/log_dest.forward.h>
#include <jackalope/log/log_engine.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

namespace jackalope {

class log_dest : public baseobj, public lockable {

protected:
    log_level min_level = log_level::uninit;

    log_level get_min_level__e() noexcept;
    virtual void handle_event__e(const log_event& event_in) noexcept = 0;
    virtual void handle_deliver__e(const log_event& event_in) noexcept;

public:
    log_dest(const log_level min_level_in);
    log_level get_min_level() noexcept;
    virtual void handle_deliver(const log_event& event_in) noexcept;
};

class log_console : public log_dest {

protected:
    mutex_type console_mutex;

public:
    log_console(const log_level min_level_in);
    void handle_event__e(const log_event& event_in) noexcept;
};

} // namespace jackalope
