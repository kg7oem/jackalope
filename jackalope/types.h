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

#include <atomic>
#include <boost/pool/pool_alloc.hpp>
#include <complex>
#include <exception>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

// g++ 6.3.0 as it comes in debian/stretch does not support maybe_unused
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED [[ maybe_unused ]]
#endif

#ifdef NDEBUG
#define NDEBUG_UNUSED UNUSED
#else
#define NDEBUG_UNUSED
#endif

namespace jackalope {

template <typename T>
using atomic_t = std::atomic<T>;
using char_t = char;
using int_t = int;
using size_t = unsigned long;
using real_t = float;
using complex_t = std::complex<real_t>;
template <typename T>
using function_t = std::function<T>;
template <typename... T>
using tuple_t = std::tuple<T...>;

template <typename T>
using pool_allocator_t = boost::pool_allocator<T>;
template <typename T>
using pool_list_t = std::list<T, pool_allocator_t<T>>;
template <class Key, class T, class Compare = std::less<Key>>
using pool_map_t = std::map<Key, T, Compare, pool_allocator_t<std::pair<const Key, T>>>;
template <typename T>
using pool_vector_t = std::vector<T, pool_allocator_t<T>>;

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

struct base_obj_t {
    base_obj_t(const base_obj_t&) = delete;
    base_obj_t(const base_obj_t&&) = delete;
    // base_obj_t& operator=(const base_obj_t&);

    base_obj_t() = default;
    virtual ~base_obj_t() = default;
};

template<typename T>
struct shared_obj_t : public base_obj_t, public std::enable_shared_from_this<T> {
    shared_t<T> shared_from_this() = delete;

    inline shared_t<T> shared_obj()
    {
        return std::enable_shared_from_this<T>::shared_from_this();
    }

    template <class U>
    shared_t<U> shared_obj()
    {
        return dynamic_pointer_cast<U>(shared_obj());
    }
};

}
