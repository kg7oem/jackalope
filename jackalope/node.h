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
#include <jackalope/exception.h>
#include <jackalope/node.forward.h>
#include <jackalope/property.h>
#include <jackalope/thread.h>
#include <jackalope/types.h>

#define JACKALOPE_NODE_PROPERTY_NAME "node:name"
#define JACKALOPE_NODE_PROPERTY_CLASS "node:class"

namespace jackalope {

using node_init_list_t = std::initializer_list<std::pair<string_t, string_t>>;

using node_constructor_t = function_t<node_t * (const string_t& name_in, node_init_list_t)>;
void add_node_constructor(const string_t& class_name_in, node_constructor_t constructor_in);
node_t * _make_node(const string_t& class_name_in, const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());

template <typename T = node_t, typename... Args>
T * make_node(Args... args)
{
    auto new_node = _make_node(args...);
    return dynamic_cast<T *>(new_node);
}

/* node life cycle
 *
 *   construct
 *   init
 *   activate
 *   start
 */

struct node_t : public baseobj_t {
    friend void input_t::notify();

    using inputs_vector_t = pool_vector_t<input_t *>;
    using outputs_vector_t = pool_vector_t<output_t *>;

    const string_t name;
    const string_t class_name;
    // init_args must preserve order
    node_init_list_t init_args;
    bool initialized_flag = false;
    bool activated_flag = false;
    bool started_flag = false;
    pool_map_t<string_t, property_t> properties;
    inputs_vector_t inputs;
    pool_map_t<string_t, input_t *> inputs_by_name;
    pool_vector_t<output_t *> outputs;
    pool_map_t<string_t, output_t *> outputs_by_name;

    template <class T = node_t>
    static T * make(node_init_list_t init_list_in)
    {
        string_t node_name;

        for(auto i : init_list_in) {
            if (i.first == JACKALOPE_NODE_PROPERTY_NAME) {
                node_name = i.second;
                break;
            }
        }

        if (node_name == "") {
            throw_runtime_error("could not find node name in init args");
        }

        return new T(node_name, init_list_in);
    }

    node_t(const string_t& name_in, node_init_list_t init_list_in = node_init_list_t());
    virtual ~node_t();
    virtual void init();
    virtual void activate();
    virtual void start();
    virtual void reset();
    virtual property_t& add_property(const string_t& name_in, property_t::type_t type_in);
    property_t& get_property(const string_t& name_in);
    virtual input_t& add_input(const string_t& channel_class_in, const string_t& name_in);
    bool has_init_arg(const string_t& arg_name_in);
    virtual string_t get_init_arg(const string_t& arg_name_in);
    virtual input_t& _get_input(const string_t& name_in);

    template <class T = input_t>
    T& get_input(const string_t& name_in)
    {
        return dynamic_cast<T&>(_get_input(name_in));
    }

    virtual const inputs_vector_t& get_inputs();
    virtual output_t& add_output(const string_t& channel_class_in, const string_t& name_in);
    virtual output_t& _get_output(const string_t& name_in);

    template <class T = output_t>
    T& get_output(const string_t& name_in)
    {
        return dynamic_cast<T&>(_get_output(name_in));
    }

    virtual const outputs_vector_t& get_outputs();
    virtual void input_ready(input_t& input_in) = 0;
    bool virtual is_initialized();
    bool virtual is_activated();
    bool virtual is_started();
    virtual const string_t& get_name();
    virtual const string_t& get_class_name();
    virtual void notify() = 0;
};

} // namespace jackalope
