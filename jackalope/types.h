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

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <complex>
#include <exception>
#include <tsl/ordered_map.h>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <vector>

namespace jackalope {

using char_t = char;
using int_t = int;
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
template<typename Key, typename T, class Compare = std::less<Key>, class Alloc = std::allocator<std::pair<Key, T>>>
using sequenced_map_t = tsl::ordered_map<Key, T>;
template <typename Key, typename T>
using pool_sequenced_map_t = sequenced_map_t<Key, T, pool_allocator_t<std::pair<const Key, T>>>;

using std::dynamic_pointer_cast;
template <typename T>
using shared_t = std::shared_ptr<T>;
template <class T, class... Args>
shared_t<T> make_shared(Args&&... args)
{
    static pool_allocator_t<T> pool;
    return std::allocate_shared<T>(pool, args...);
}
template <class T>
using weak_t = std::weak_ptr<T>;

template<typename T>
//using shared_obj_t = std::enable_shared_from_this<T>;
struct shared_obj_t : public std::enable_shared_from_this<T> {
    shared_t<T> shared_from_this() = delete;

    template <class U = T, typename... Args>
    shared_t<U> shared_obj(Args... args)
    {
        return dynamic_pointer_cast<U>(std::enable_shared_from_this<T>::shared_from_this());
    }
};

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
