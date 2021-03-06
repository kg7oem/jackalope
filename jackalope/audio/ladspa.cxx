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

#include <cmath>
#include <cstdlib>
#include <dlfcn.h>
#include <iostream>

#include <boost/filesystem.hpp>

#include <jackalope/audio/ladspa.h>
#include <jackalope/exception.h>
#include <jackalope/jackalope.h>
#include <jackalope/logging.h>
#include <jackalope/pcm.h>
#include <jackalope/string.h>

#define LADSPA_DESCRIPTOR_SYMBOL "ladspa_descriptor"

namespace jackalope {

namespace audio {

static string_t ladspa_path;

static shared_t<ladspa_node_t> ladspa_node_constructor(NDEBUG_UNUSED const string_t& type_in, const init_args_t init_args_in)
{
    assert(type_in == JACKALOPE_AUDIO_LADSPA_OBJECT_TYPE);

    return jackalope::make_shared<ladspa_node_t>(init_args_in);
}

void ladspa_init()
{
    auto from_env = std::getenv(JACKALOPE_PCM_LADSPA_PATH_ENV);

    if (from_env == nullptr) {
        ladspa_path = JACKALOPE_PCM_LADSPA_PATH_DEFAULT;
    } else {
        ladspa_path = from_env;
    }

    add_object_constructor(JACKALOPE_AUDIO_LADSPA_OBJECT_TYPE, ladspa_node_constructor);
}

ladspa_node_t::ladspa_node_t(const init_args_t init_args_in)
: filter_plugin_t(init_args_in)
{ }

ladspa_node_t::~ladspa_node_t()
{
    if (file != nullptr) {
        delete file;
        file = nullptr;
    }
}

static ladspa_file_t * make_file_by_id(const ladspa_id_t id_in)
{
    for(auto i : split_string(ladspa_path, ':')) {
        boost::filesystem::path search_dir(i.c_str());

        if (! boost::filesystem::is_directory(search_dir)) {
            continue;
        }

        for(auto& j : boost::filesystem::directory_iterator(search_dir)) {
            auto path_string = string_t(boost::filesystem::canonical(j).string());
            auto ladspa_file = new ladspa_file_t(path_string);

            for(auto k : ladspa_file->get_descriptors()) {
                if (k->UniqueID == id_in) {
                    return ladspa_file;
                }
            }

            delete ladspa_file;
        }
    }

    throw_runtime_error("could not find LADSPA file for type: ", id_in);
}

void ladspa_node_t::init()
{
    assert_lockable_owner();

    filter_plugin_t::init();

    add_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, property_t::type_t::size, init_args);
    add_property(JACKALOPE_PCM_LADSPA_PROPERTY_FILE, property_t::type_t::string, init_args);
    add_property(JACKALOPE_PCM_LADSPA_PROPERTY_ID, property_t::type_t::size, init_args);
}

void ladspa_node_t::init_file()
{
    assert_lockable_owner();

    assert(init_flag);

    auto type_property = get_property(JACKALOPE_PCM_LADSPA_PROPERTY_ID);
    auto file_property = get_property(JACKALOPE_PCM_LADSPA_PROPERTY_FILE);

    auto type_is_defined = type_property->is_defined();
    auto file_is_defined = file_property->is_defined();

    if(! type_is_defined && ! file_is_defined) {
        throw_runtime_error("no LADSPA type and no filename was specified");
    } else if(! file_is_defined) {
        file = make_file_by_id(type_property->get_size());
        file_property->set_string(file->path);
    } else if (! type_is_defined) {
        file = new ladspa_file_t(file_property->get_string());

        auto descriptors = file->get_descriptors();

        if (descriptors.size() != 1) {
            throw_runtime_error("LADSPA file had more than one ID and no ID was specified");
        }

        type_property->set_size(descriptors[0]->UniqueID);
    }
}

void ladspa_node_t::init_instance()
{
    assert_lockable_owner();

    assert(init_flag);

    instance = new ladspa_instance_t(*file, get_property(JACKALOPE_PCM_LADSPA_PROPERTY_ID)->get_size());

    for(size_t port_num = 0; port_num < instance->get_num_ports(); port_num++) {
        auto descriptor = instance->get_port_descriptor(port_num);
        auto port_name = instance->get_port_name(port_num);

        if (LADSPA_IS_PORT_CONTROL(descriptor)) {
            if (LADSPA_IS_PORT_INPUT(descriptor)) {
                auto property_name = to_string("config.", port_name);
                add_property(property_name, property_t::type_t::real);
            } else if (LADSPA_IS_PORT_OUTPUT(descriptor)) {
                auto property_name = to_string("state.", port_name);
                add_property(property_name, property_t::type_t::real)->set(0);
            }
        } else if(LADSPA_IS_PORT_AUDIO(descriptor)) {
            if (LADSPA_IS_PORT_INPUT(descriptor)) {
                add_sink(port_name, JACKALOPE_TYPE_AUDIO);
            } else if (LADSPA_IS_PORT_OUTPUT(descriptor)) {
                add_source(port_name, JACKALOPE_TYPE_AUDIO);
            }
        }
    }
}

void ladspa_node_t::activate()
{
    assert_lockable_owner();

    for (auto i : { JACKALOPE_PROPERTY_PCM_SAMPLE_RATE, JACKALOPE_PROPERTY_PCM_BUFFER_SIZE }) {
        set_undef_property(i);
    }

    filter_plugin_t::activate();

    auto sample_rate_prop = get_property(JACKALOPE_PROPERTY_PCM_SAMPLE_RATE);
    auto sample_rate = sample_rate_prop->get_size();

    init_file();
    init_instance();

    instance->instantiate(sample_rate);
    instance->activate();

    for(size_t port_num = 0; port_num < instance->get_num_ports(); port_num++) {
        auto descriptor = instance->get_port_descriptor(port_num);
        auto port_name = instance->get_port_name(port_num);

        if (LADSPA_IS_PORT_CONTROL(descriptor)) {
            if (LADSPA_IS_PORT_INPUT(descriptor)) {
                auto property_name = to_string("config.", port_name);
                auto property = get_property(property_name);

                if (! property->is_defined()) {
                    property->set(instance->get_port_default(port_num));
                }

                instance->connect_port(port_num, &property->get_real());
            } else if (LADSPA_IS_PORT_OUTPUT(descriptor)) {
                auto property_name = to_string("state.", port_name);
                auto property = get_property(property_name);

                property->set(0);
                instance->connect_port(port_num, &property->get_real());
            }
        }
    }
}

void ladspa_node_t::execute()
{
    assert_lockable_owner();

    assert(started_flag);
    assert(! stopped_flag);

    auto buffer_size = get_property(JACKALOPE_PROPERTY_PCM_BUFFER_SIZE)->get_size();
    pool_map_t<string_t, shared_t<audio_buffer_t>> source_buffers;

    for(size_t port_num = 0; port_num < instance->get_num_ports(); port_num++) {
        auto descriptor = instance->get_port_descriptor(port_num);

        if (LADSPA_IS_PORT_AUDIO(descriptor)) {
            auto port_name = instance->get_port_name(port_num);

            if(LADSPA_IS_PORT_INPUT(descriptor)) {
                auto sink = get_sink<audio_sink_t>(port_name);
                auto buffer = sink->get_buffer();
                instance->connect_port(port_num, buffer->get_pointer());
            } else if (LADSPA_IS_PORT_OUTPUT(descriptor)) {
                auto buffer = jackalope::make_shared<audio_buffer_t>(buffer_size);
                source_buffers[port_name] = buffer;
                instance->connect_port(port_num, buffer->get_pointer());
            }
        }
    }

    instance->run(buffer_size);

    for(size_t port_num = 0; port_num < instance->get_num_ports(); port_num++) {
        auto descriptor = instance->get_port_descriptor(port_num);

        if(LADSPA_IS_PORT_AUDIO(descriptor)) {
            auto port_name = instance->get_port_name(port_num);

            instance->connect_port(port_num, nullptr);

            if (LADSPA_IS_PORT_INPUT(descriptor)) {
                auto sink = get_sink<audio_sink_t>(port_name);
                sink->reset();
            } else if (LADSPA_IS_PORT_OUTPUT(descriptor)) {
                auto buffer = source_buffers[port_name];
                auto source = get_source<audio_source_t>(port_name);
                source->notify_buffer(buffer);
            }
        }
    }
}

ladspa_file_t::ladspa_file_t(const string_t& path_in)
: path(path_in)
{
    handle = dlopen(path_in.c_str(), RTLD_NOW);

    if (handle == nullptr) {
        throw_runtime_error("could not dlopen(", path, "): ", dlerror());
    }

    descriptor_fn = (ladspa_descriptor_function_t) dlsym(handle, LADSPA_DESCRIPTOR_SYMBOL);
    if (descriptor_fn == nullptr) {
        throw_runtime_error("could not get descriptor function for ", path);
    }

    for (auto&& i : get_descriptors()) {
        id_to_descriptor[i->UniqueID] = i;
    }
}

ladspa_file_t::~ladspa_file_t()
{
    if (handle != nullptr) {
        if (dlclose(handle)) {
            // FIXME there should be a panic() tool
            jackalope_panic("could not dlclose() ladspa plugin handle:", dlerror());
            abort();
        }

        handle = nullptr;
        descriptor_fn = nullptr;
    }
}

const pool_vector_t<const ladspa_descriptor_t *> ladspa_file_t::get_descriptors()
{
    pool_vector_t<const ladspa_descriptor_t *> descriptors;
    const ladspa_descriptor_t * p;

    for(long i = 0; (p = descriptor_fn(i)) != nullptr; i++) {
        descriptors.push_back(p);
    }

    return descriptors;
}

const ladspa_descriptor_t * ladspa_file_t::get_descriptor(const ladspa_id_t id_in)
{
    auto found = id_to_descriptor.find(id_in);

    if (found == id_to_descriptor.end()) {
        throw_runtime_error("Could not find descriptor for LADSPA ID: ", id_in);
    }

    return found->second;
}

ladspa_instance_t::ladspa_instance_t(ladspa_file_t& file_in, const ladspa_id_t id_in)
: file(file_in), id(id_in)
{
    descriptor = file.get_descriptor(id_in);

    for(size_t i = 0; i < get_num_ports(); i++) {
        port_name_to_num[get_port_name(i)] = i;
    }
}

ladspa_instance_t::~ladspa_instance_t()
{
    if (handle != nullptr) {
        descriptor->deactivate(handle);
        descriptor->cleanup(handle);

        handle = nullptr;
    }
}

size_t ladspa_instance_t::get_num_ports()
{
    return descriptor->PortCount;
}

ladspa_port_descriptor_t ladspa_instance_t::get_port_descriptor(const size_t port_num_in)
{
    if (port_num_in >= get_num_ports()) {
        throw_runtime_error("port number was greater than LADSPA port count: ", port_num_in);
    }

    return descriptor->PortDescriptors[port_num_in];
}

const string_t ladspa_instance_t::get_port_name(const size_t port_num_in)
{
    if (port_num_in >= get_num_ports()) {
        throw_runtime_error("port number was greater than LADSPA port count: ", port_num_in);
    }

    return descriptor->PortNames[port_num_in];
}

size_t ladspa_instance_t::get_port_num(const string_t& port_name_in)
{
    auto found = port_name_to_num.find(port_name_in);

    if (found == port_name_to_num.end()) {
        throw_runtime_error("Could not find LADSPA port number: ", port_name_in);
    }

    return found->second;
}

ladspa_data_t ladspa_instance_t::get_port_default(const size_t port_num_in)
{
    if (port_num_in >= get_num_ports()) {
        throw_runtime_error("port number was greater than LADSPA port count: ", port_num_in);
    }

    auto port_hints = descriptor->PortRangeHints[port_num_in];
    auto hint_descriptor = port_hints.HintDescriptor;

    if (! LADSPA_IS_HINT_HAS_DEFAULT(hint_descriptor)) {
        return 0;
    } else if (LADSPA_IS_HINT_DEFAULT_0(hint_descriptor)) {
        return 0;
    } else if (LADSPA_IS_HINT_DEFAULT_1(hint_descriptor)) {
        return 1;
    } else if (LADSPA_IS_HINT_DEFAULT_100(hint_descriptor)) {
        return 100;
    } else if (LADSPA_IS_HINT_DEFAULT_440(hint_descriptor)) {
        return 440;
    } else if (LADSPA_IS_HINT_DEFAULT_MINIMUM(hint_descriptor)) {
        return port_hints.LowerBound;
    } else if (LADSPA_IS_HINT_DEFAULT_LOW(hint_descriptor)) {
        if (LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)) {
            return std::exp(std::log(port_hints.LowerBound) * 0.75 + std::log(port_hints.UpperBound) * 0.25);
        } else {
            return port_hints.LowerBound * 0.75 + port_hints.UpperBound * 0.25;
        }
    } else if (LADSPA_IS_HINT_DEFAULT_MIDDLE(hint_descriptor)) {
        if (LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)) {
            return std::exp(std::log(port_hints.LowerBound) * 0.5 + std::log(port_hints.UpperBound) * 0.5);
        } else {
            return (port_hints.LowerBound * 0.5 + port_hints.UpperBound * 0.5);
        }
    } else if (LADSPA_IS_HINT_DEFAULT_HIGH(hint_descriptor)) {
        if (LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)) {
            return std::exp(std::log(port_hints.LowerBound) * 0.25 + std::log(port_hints.UpperBound) * 0.75);
        } else {
            return port_hints.LowerBound * 0.25 + port_hints.UpperBound * 0.75;
        }
    } else if (LADSPA_IS_HINT_DEFAULT_MAXIMUM(hint_descriptor)) {
        return port_hints.UpperBound;
    }

    throw_runtime_error("could not find hint for LADSPA port: ", port_num_in);
}

void ladspa_instance_t::instantiate(const size_t sample_rate_in)
{
    handle = descriptor->instantiate(descriptor, sample_rate_in);

    if (handle == nullptr) {
        throw_runtime_error("could not instantiate LADSPA id: ", id);
    }
}

void ladspa_instance_t::activate()
{
    assert(handle != nullptr);

    if (descriptor->activate != nullptr) {
        descriptor->activate(handle);
    }
}

void ladspa_instance_t::run(const size_t num_samples_in)
{
    assert(handle != nullptr);

    descriptor->run(handle, num_samples_in);
}

void ladspa_instance_t::connect_port(const size_t port_num_in, ladspa_data_t * pointer_in)
{
    assert(handle != nullptr);

    descriptor->connect_port(handle, port_num_in, pointer_in);
}

} // namespace pcm

} // namespace jackalope
