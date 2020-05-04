//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <limits>

/**
 * A custom allocator class that allows de/allocation-free std::move for e.g. std::vector
 *
 * Adapted from http://www.josuttis.com/libbook/memory/Allocator.hpp.html
 */
template <class T>
class Allocator
{
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    
    Allocator() = default;
    Allocator(const Allocator&) = default;
    ~Allocator() = default;
    
    /** Copy constructor - empty because Allocator has no state */
    template <class U>
    Allocator(const Allocator<U>&) {}

    // rebind allocator to type U
    template <class U>
    struct rebind
    {
        typedef Allocator<U> other;
    };

    pointer address(reference value) const { return &value; }
    const_pointer address(const_reference value) const { return &value; }

    // return maximum number of elements that can be allocated
    size_type max_size() const { return std::numeric_limits<std::size_t>::max() / sizeof(T); }

    // allocate but don't initialize num elements of type T
    pointer allocate(size_type num, const void* = 0)
    {
        return static_cast<pointer>(::operator new(num * sizeof(T)));
    }

    // initialize elements of allocated storage p with value value
    void construct(pointer p, const T& value)
    {
        new(static_cast<void*>(p)) T(value); // placement new
    }

    // destroy elements of initialized storage p
    void destroy(pointer p)
    {
        p->~T(); // explicitly call destructor
    }

    // deallocate storage p of deleted elements
    void deallocate(pointer p, size_type num)
    {
        (void)num; // quiet compiler
        ::operator delete(static_cast<void*>(p));
    }
};

// return that all specializations of this allocator are interchangeable
template <class T1, class T2>
bool operator== (const Allocator<T1>&, const Allocator<T2>&) throw() { return true; }
template <class T1, class T2>
bool operator!= (const Allocator<T1>&, const Allocator<T2>&) throw() { return false; }

