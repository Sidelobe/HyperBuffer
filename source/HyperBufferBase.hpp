//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <memory>
#include <vector>

#include "TemplateUtils.hpp"
#include "IntArrayOperations.hpp"

// Macros to restrict a function declaration to 1-dimensional and higher-dimensional case only
#define FOR_N1 template<int M=N, std::enable_if_t<(M==1), int> = 0>
#define FOR_Nx template<int M=N, std::enable_if_t<(M>1), int> = 0>
#define FOR_Nx_V template<int M=N, typename... I, std::enable_if_t<(M>1), int> = 0>


/**
 * This has to be a base and interface class at once, because we cannot apply std::enable_if to virtual functions and
 * we need the former for this to work in C++14.
 */
template<typename T, int N>
class HyperBufferBase
{
protected:
    using size_type = int;
    using pointer_type = typename add_pointers_to_type<T, N>::type;
    using subdim_pointer_type = typename remove_pointers_from_type<pointer_type, 1>::type;

    // Helper to make interfacing with STL a bit more readable
    using stl_size_type = typename std::vector<T*>::size_type;
    
public:
    virtual ~HyperBufferBase() = default;
    
    // MARK: dimension extents
    int dim(int i) const { assert(i < N); return m_dimensionExtents[STL(i)]; }
    const std::array<int, N>& dims() const { return m_dimensionExtents; }

    // MARK: operator[]
    FOR_Nx subdim_pointer_type operator[] (size_type i) { return getDataPointer_Nx()[i]; }
    FOR_Nx const subdim_pointer_type operator[] (size_type i) const { return getDataPointer_Nx()[i]; }
    FOR_N1 T& operator[] (size_type i) { return getDataPointer_N1()[i]; }
    FOR_N1 const T& operator[] (size_type i) const { return getDataPointer_N1()[i]; }

    // MARK: data()
    // NOTE: We cannot make these virtual functions because of the differente return types required.
    // decltype(auto) is not allowed for virtual functions, so I chose an enable_if construct for selective compilation
    FOR_Nx pointer_type data() { return getDataPointer_Nx(); }
    FOR_Nx const pointer_type data() const { return getDataPointer_Nx(); }
    FOR_N1 T* data() { return getDataPointer_N1(); }
    FOR_N1 const T* data() const { return getDataPointer_N1(); }
    
protected:
    // MARK: constructors
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBufferBase(I... i) : m_dimensionExtents{static_cast<int>(i)...}
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }
    
    /** Constructor that takes the extents of the dimensions as a std::array */
    explicit HyperBufferBase(const std::array<int, N>& dimensionExtents) : m_dimensionExtents{dimensionExtents} {}
    
    /** Constructor that takes the extents of the dimensions as a std::vector */
    explicit HyperBufferBase(const std::vector<int>& dimensionExtents)
    {
        std::copy(dimensionExtents.begin(), dimensionExtents.end(), m_dimensionExtents.begin());
    }

    // MARK: Virtual functions to be defined by derived classes
    virtual const pointer_type getDataPointer_Nx() const = 0;
    virtual pointer_type getDataPointer_Nx() = 0;
    virtual const T* getDataPointer_N1() const = 0;
    virtual T* getDataPointer_N1() = 0;

    // Helper to make interfacing with STL a bit more readable
    static constexpr stl_size_type STL(int i) { return static_cast<stl_size_type>(i); }
    
    // https://stackoverflow.com/questions/5695548/public-friend-swap-member-function
    friend void swap(HyperBufferBase<T, N>& first, HyperBufferBase<T, N>& second) noexcept
    {
        using std::swap; // allow use of std::swap
        swap(first.m_dimensionExtents, second.m_dimensionExtents); // but select overloads, first
        // if swap(x, y) finds a better match, via ADL, it will use that instead; otherwise it falls back to std::swap
    }

protected:
    std::array<int, N> m_dimensionExtents; // only required by the dims functions

};

