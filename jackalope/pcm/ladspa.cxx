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

#include <jackalope/audio.h>
#include <jackalope/exception.h>
#include <jackalope/jackalope.h>
#include <jackalope/logging.h>
#include <jackalope/pcm/ladspa.h>
#include <jackalope/string.h>

#define LADSPA_DESCRIPTOR_SYMBOL "ladspa_descriptor"

namespace jackalope {

namespace pcm {

static string_t ladspa_path;

static shared_t<ladspa_node_t> ladspa_node_constructor(const string_t& node_name_in, node_init_list_t init_list_in)
{
    return jackalope::make_shared<ladspa_node_t>(node_name_in, init_list_in);
}

void ladspa_init()
{
    auto from_env = std::getenv(JACKALOPE_PCM_LADSPA_PATH_ENV);

    if (from_env == nullptr) {
        ladspa_path = JACKALOPE_PCM_LADSPA_PATH_DEFAULT;
    } else {
        ladspa_path = from_env;
    }

    add_node_constructor(JACKALOPE_PCM_LADSPA_CLASS, ladspa_node_constructor);
}

ladspa_node_t::ladspa_node_t(const string_t& node_name_in, node_init_list_t init_list_in)
: audio_node_t(node_name_in, init_list_in)
{
    add_property(JACKALOPE_PCM_LADSPA_PROPERTY_FILE, property_t::type_t::string);
    add_property(JACKALOPE_PCM_LADSPA_PROPERTY_ID, property_t::type_t::size);
}

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
    init_file();
    init_instance();

    audio_node_t::init();
}

void ladspa_node_t::init_file()
{
    auto& type_property = get_property(JACKALOPE_PCM_LADSPA_PROPERTY_ID);
    auto type_is_defined = type_property.is_defined();
    auto& file_property = get_property(JACKALOPE_PCM_LADSPA_PROPERTY_FILE);
    auto file_is_defined = file_property.is_defined();

    if(! type_is_defined && ! file_is_defined) {
        throw_runtime_error("no LADSPA type and no filename was specified");
    } else if(! file_is_defined) {
        file = make_file_by_id(type_property.get_size());
        file_property.set_string(file->path);
    } else if (! type_is_defined) {
        file = new ladspa_file_t(file_property.get_string());

        auto descriptors = file->get_descriptors();

        if (descriptors.size() != 1) {
            throw_runtime_error("LADSPA file had more than one ID and no ID was specified");
        }

        type_property.set_size(descriptors[0]->UniqueID);
    }
}

void ladspa_node_t::init_instance()
{
    instance = new ladspa_instance_t(*file, get_property(JACKALOPE_PCM_LADSPA_PROPERTY_ID).get_size());

    for(size_t port_num = 0; port_num < instance->get_num_ports(); port_num++) {
        auto descriptor = instance->get_port_descriptor(port_num);
        auto port_name = instance->get_port_name(port_num);

        if (LADSPA_IS_PORT_CONTROL(descriptor)) {
            if (LADSPA_IS_PORT_INPUT(descriptor)) {
                auto property_name = to_string("config:", port_name);
                add_property(property_name, property_t::type_t::real);
            } else if (LADSPA_IS_PORT_OUTPUT(descriptor)) {
                auto property_name = to_string("state:", port_name);
                add_property(property_name, property_t::type_t::real).set(0);
            }
        } else if(LADSPA_IS_PORT_AUDIO(descriptor)) {
            if (LADSPA_IS_PORT_INPUT(descriptor)) {
                add_input(JACKALOPE_PCM_CHANNEL_CLASS_REAL, port_name);
            } else if (LADSPA_IS_PORT_OUTPUT(descriptor)) {
                add_output(JACKALOPE_PCM_CHANNEL_CLASS_REAL, port_name);
            }
        }
    }
}

void ladspa_node_t::activate()
{
    auto& sample_rat_prop = get_property(JACKALOPE_PCM_PROPERTY_SAMPLE_RATE);
    auto& buffer_size_prop = get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE);

    for (auto i : outputs) {
        auto pcm_output = dynamic_pointer_cast<pcm_real_output_t>(i);
        pcm_output->set_num_samples(buffer_size_prop.get_size());
    }

    instance->instantiate(sample_rat_prop.get_size());
    instance->activate();

    for(size_t port_num = 0; port_num < instance->get_num_ports(); port_num++) {
        auto descriptor = instance->get_port_descriptor(port_num);
        auto port_name = instance->get_port_name(port_num);

        if (LADSPA_IS_PORT_CONTROL(descriptor)) {
            if (LADSPA_IS_PORT_INPUT(descriptor)) {
                auto property_name = to_string("config:", port_name);
                auto& property = get_property(property_name);

                property.set(instance->get_port_default(port_num));
                instance->connect_port(port_num, &property.get_real());
            } else if (LADSPA_IS_PORT_OUTPUT(descriptor)) {
                auto property_name = to_string("state:", port_name);
                auto& property = get_property(property_name);

                property.set(0);
                instance->connect_port(port_num, &property.get_real());
            }
        } else if(LADSPA_IS_PORT_AUDIO(descriptor)) {
            // inputs get connected right before the LADSPA plugin is run
            if (LADSPA_IS_PORT_OUTPUT(descriptor)) {
                auto port_name = instance->get_port_name(port_num);
                auto output = get_output<pcm_real_output_t>(port_name);
                instance->connect_port(port_num, output->get_buffer_pointer());
            }
        }
    }

    audio_node_t::activate();
}

void ladspa_node_t::pcm_ready()
{
    audio_node_t::pcm_ready();

    for(size_t port_num = 0; port_num < instance->get_num_ports(); port_num++) {
        auto descriptor = instance->get_port_descriptor(port_num);

        if(LADSPA_IS_PORT_AUDIO(descriptor) && LADSPA_IS_PORT_INPUT(descriptor)) {
            auto input = get_input<pcm_real_input_t>(instance->get_port_name(port_num));
            instance->connect_port(port_num, input->get_buffer_pointer());
        }
    }

    instance->run(get_property(JACKALOPE_PCM_PROPERTY_BUFFER_SIZE).get_size());

    for(size_t port_num = 0; port_num < instance->get_num_ports(); port_num++) {
        auto descriptor = instance->get_port_descriptor(port_num);

        if(LADSPA_IS_PORT_AUDIO(descriptor) && LADSPA_IS_PORT_INPUT(descriptor)) {
            instance->connect_port(port_num, nullptr);
        }
    }

    for(auto i : outputs) {
        i->set_dirty();
    }

    notify();
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

    descriptor->activate(handle);
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
