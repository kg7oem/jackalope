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
#include <complex>
#include <exception>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <vector>

namespace jackalope {

using char_t = char;
using size_t = unsigned long;
using real_t = float;
using complex_t = std::complex<real_t>;
template <typename T>
using function_t = std::function<T>;

template <typename T>
using pool_allocator_t = boost::pool_allocator<T>;
template <typename T>
using pool_list_t = std::list<T, pool_allocator_t<T>>;
template <class Key, class T, class Compare = std::less<Key>>
using pool_map_t = std::map<Key, T, Compare, pool_allocator_t<std::pair<const Key, T>>>;
template <typename T>
using pool_vector_t = std::vector<T, pool_allocator_t<T>>;

template <typename T>
using shared_t = std::shared_ptr<T>;
template <class T, class... Args>
shared_t<T> make_shared(Args&&... args)
{
    static pool_allocator_t<T> pool;
    return std::allocate_shared<T>(pool, args...);
}

struct runtime_error_t : public std::runtime_error {
    runtime_error_t(const std::string& what_in);
    runtime_error_t(const char * what_in);
};

struct baseobj_t {
    baseobj_t(const baseobj_t&) = delete;
    baseobj_t(const baseobj_t&&) = delete;
    baseobj_t& operator=(const baseobj_t&);

    baseobj_t() = default;
    virtual ~baseobj_t() = default;
};

}
