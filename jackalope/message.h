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

#include <jackalope/string.h>
#include <jackalope/types.h>

namespace jackalope {

class abstract_message_t : public base_t, public shared_obj_t<abstract_message_t> {

public:
    const string_t name;

    abstract_message_t(const string_t& name_in);
};

class abstract_message_handler_t : public base_t, public shared_obj_t<abstract_message_handler_t> {

public:
    virtual void invoke(shared_t<abstract_message_t> message_in) = 0;
};

template <typename... T>
class message_t : public abstract_message_t {

public:
    using args_t = tuple_t<T...>;
    using handler_t = function_t<void (T...)>;

    class message_handler_t : public abstract_message_handler_t {

    protected:
        handler_t handler;

    public:
        message_handler_t(handler_t handler_in)
        : handler(handler_in)
        { }

        virtual void invoke(shared_t<abstract_message_t> message_in) {
            auto typed_message = dynamic_pointer_cast<message_t>(message_in);
            std::apply(handler, typed_message->args);
        }
    };

    const args_t args;

protected:
    message_t(const string_t& name_in, T... args)
    : abstract_message_t(name_in), args(args_t(args...))
    { }
};

} //namespace jackalope
