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

#include <cstdlib>
#include <dlfcn.h>
#include <iostream>

#include <boost/filesystem.hpp>

#include <jackalope/audio/ladspa.h>
#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/string.h>

#define LADSPA_DESCRIPTOR_SYMBOL "ladspa_descriptor"

namespace jackalope {

namespace audio {

static string_t ladspa_path;

static ladspa_node_t * ladspa_node_constructor(const string_t& node_name_in)
{
    return new ladspa_node_t(node_name_in);
}

void ladspa_init()
{
    auto from_env = std::getenv(JACKALOPE_AUDIO_LADSPA_PATH_ENV);

    if (from_env == nullptr) {
        ladspa_path = JACKALOPE_AUDIO_LADSPA_PATH_DEFAULT;
    } else {
        ladspa_path = from_env;
    }

    add_audio_node_constructor(JACKALOPE_AUDIO_LADSPA_CLASS, ladspa_node_constructor);
}

ladspa_node_t::ladspa_node_t(const string_t& node_name_in)
: audio_node_t(node_name_in, JACKALOPE_AUDIO_LADSPA_CLASS)
{
    add_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_FILE, property_t::type_t::string);
    add_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_TYPE, property_t::type_t::size);
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
    auto& type_property = get_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_TYPE);
    auto type_is_defined = type_property.is_defined();
    auto& file_property = get_property(JACKALOPE_AUDIO_LADSPA_PROPERTY_FILE);
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

    audio_node_t::init();
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
            std::cerr << "could not dlclose() ladspa plugin handle: " << dlerror() << std::endl;
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

} // namespace audio

} // namespace jackalope
