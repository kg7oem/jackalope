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
#include <jackalope/object.h>

namespace jackalope {

shared_t<source_t> object_t::add_source(const string_t& source_name_in)
{
    assert_lockable_owner();

    auto found = sources_by_name.find(source_name_in);

    if (found != sources_by_name.end()) {
        throw_runtime_error("Can not add duplicate source name: ", source_name_in);
    }

    auto new_source = jackalope::make_shared<source_t>(source_name_in, shared_obj());
    sources.push_back(new_source);
    sources_by_name[new_source->name] = new_source;

    return new_source;
}

shared_t<source_t> object_t::get_source(const string_t& source_name_in)
{
    assert_lockable_owner();

    auto found = sources_by_name.find(source_name_in);

    if (found == sources_by_name.end()) {
        throw_runtime_error("Unknown source name: ", source_name_in);
    }

    return found->second;
}

shared_t<source_t> object_t::get_source(const size_t source_num_in)
{
    assert_lockable_owner();

    if (source_num_in >= sources.size()) {
        throw_runtime_error("Source number is out of bounds: ", source_num_in);
    }

    return sources[source_num_in];
}

void object_t::init()
{
    assert_lockable_owner();
}

void object_t::start()
{
    assert_lockable_owner();
}

void object_t::stop()
{
    assert_lockable_owner();
}

void object_t::source_available(shared_t<source_t>)
{
    assert_lockable_owner();
}

void object_t::slot_source_available(shared_t<source_t> available_source_in)
{
    auto lock = get_object_lock();

    source_available(available_source_in);
}

} //namespace jackalope
