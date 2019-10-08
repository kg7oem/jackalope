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

#include <boost/pool/pool_alloc.hpp>
#include <list>
#include <map>
#include <memory>
#include <vector>

namespace jackalope {

using char_type = char;
using size_type = unsigned long;

template <typename T>
using allocator_type = boost::pool_allocator<T>;
template <typename T>
using pool_list_type = std::list<T, allocator_type<T>>;
template <class Key, class T, class Compare = std::less<Key>>
using pool_map_type = std::map<Key, T, Compare, allocator_type<std::pair<const Key, T>>>;
template <typename T>
using pool_vector_type = std::vector<T, allocator_type<T>>;

template <typename T>
using shared_type = std::shared_ptr<T>;
template <class T, class... Args>
shared_type<T> make_shared(Args&&... args) noexcept
{
    static allocator_type<T> pool;
    return std::allocate_shared<T>(pool, args...);
}

struct baseobj {
    baseobj(const baseobj&) = delete;
    baseobj(const baseobj&&) = delete;
    baseobj& operator=(const baseobj&);

    baseobj() = default;
    ~baseobj() = default;
};

}
