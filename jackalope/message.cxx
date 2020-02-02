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

#include <jackalope/exception.h>
#include <jackalope/message.h>

namespace jackalope {

abstract_message_t::abstract_message_t(const string_t& name_in)
: name(name_in)
{
    assert(name != "");
}

shared_t<abstract_message_handler_t> message_obj_t::get_message_handler(const string_t& name_in)
{
    lock_t message_lock(message_mutex);

    auto found = message_handlers.find(name_in);

    if (found == message_handlers.end()) {
        throw_runtime_error("could not find message handler: ", name_in);
    }

    return found->second;
}

void message_obj_t::deliver_messages()
{
    while(1) {
        shared_t<abstract_message_t> message;

        {
            lock_t message_lock(message_mutex);

            assert(message_delivering_flag == true);

            if (message_queue.empty()) {
                message_delivering_flag = false;
                return;
            }

            message = message_queue.front();
            message_queue.pop_front();
        }

        deliver_one_message(message);
    }
}

void message_obj_t::deliver_one_message(shared_t<abstract_message_t> message_in)
{
    auto message_name = message_in->name;
    auto message_handler = get_message_handler(message_name);

    message_handler->invoke(message_in);
}

} //namespace jackalope
