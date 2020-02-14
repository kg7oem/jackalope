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

#include <jackalope/exception.h>
#include <jackalope/logging.h>
#include <jackalope/module.h>

#ifdef MODULE_AUDIO_LINK_STATIC
#include <jackalope/audio/module.h>
#endif

namespace jackalope {

static mutex_t module_mutex;
pool_map_t<string_t, module_info_t *> loaded_modules;

void module_init()
{

#ifdef MODULE_AUDIO_LINK_STATIC
    module_load(audio_module_info_constructor());
#endif

}

void module_load(module_info_t * info_in)
{
    auto name = info_in->get_name();

    log_info("Loading module: ", name);

    lock_t module_lock(module_mutex);

    if (loaded_modules.count(name)) {
        throw_runtime_error("Duplicate module name during load: ", name);
    }

    for(auto i : info_in->get_plugin_constructors()) {
        add_plugin_constructor(i.first, i.second);
    }
}

} //namespace jackalope
