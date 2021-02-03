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

namespace slb
{

/**
 * This has to be a base and interface class at once, because we cannot apply std::enable_if to virtual functions and
 * we need the former for this to work in C++14.
 */
template<typename T, int N, typename Derived=int> // CRTP with dummy default argument (Derived=int will never be used)
class HyperBufferBase
{
protected:
    using size_type = int;
    using pointer_type = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type = typename add_const_pointers_to_type<T, N>::type;
    using subdim_pointer_type = typename remove_pointers_from_type<pointer_type, 1>::type;
    using subdim_const_pointer_type = typename remove_pointers_from_type<const_pointer_type, 1>::type;

    // Helper to make interfacing with STL a bit more readable
    using stl_size_type = typename std::vector<T*>::size_type;
    
public:
    virtual ~HyperBufferBase() = default;
    
    // MARK: dimension extents
    int dim(int i) const { ASSERT(i < N); return m_dimensionExtents[STL(i)]; }
    const std::array<int, N>& dims() const { return m_dimensionExtents; }

    // NOTE: We cannot make these virtual functions because of the different return types required.
    // Overloading by return type is not allowed, so we need an enable_if construct for selective compilation
    // MARK: data() -- raw pointer to beginning of underlying storage (pointers or data, depending on dimension)
    FOR_Nx const_pointer_type data() const { return getDataPointer_Nx(); }
    FOR_Nx       pointer_type data()       { return getDataPointer_Nx(); }
    FOR_N1 const T* data() const { return getDataPointer_N1(); }
    FOR_N1       T* data()       { return getDataPointer_N1(); }
    
    // MARK: operator[] -- raw data/pointer access; returns pointer N>1, reference for N=1
    FOR_Nx subdim_const_pointer_type operator[] (size_type i) const { return getDataPointer_Nx()[i]; }
    FOR_Nx       subdim_pointer_type operator[] (size_type i)       { return getDataPointer_Nx()[i]; }
    FOR_N1 const T& operator[] (size_type i) const { return getDataPointer_N1()[i]; }
    FOR_N1       T& operator[] (size_type i)       { return getDataPointer_N1()[i]; }
    
    // MARK: at() -- data/subbuffer access; returns Derived<T,N-1> instance or data
    FOR_Nx_V decltype(auto) at (size_type dn, I... i) const { return static_cast<const Derived*>(this)->createSubBufferView(dn).at(i...); }
    FOR_Nx_V decltype(auto) at (size_type dn, I... i)       { return static_cast<Derived*>(this)->createSubBufferView(dn).at(i...); }
    FOR_Nx decltype(auto) at(size_type dn) const { return static_cast<const Derived*>(this)->createSubBufferView(dn); }
    FOR_Nx decltype(auto) at (size_type dn)      { return static_cast<Derived*>(this)->createSubBufferView(dn); }
    FOR_N1 const T& at (size_type i) const { return getDataPointer_N1()[i]; }
    FOR_N1       T& at (size_type i)       { return getDataPointer_N1()[i]; }

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
    virtual const_pointer_type getDataPointer_Nx() const = 0;
    virtual       pointer_type getDataPointer_Nx() = 0;
    virtual const T* getDataPointer_N1() const = 0;
    virtual       T* getDataPointer_N1() = 0;

    // Helper to make interfacing with STL a bit more readable
    static constexpr stl_size_type STL(int i) { return static_cast<stl_size_type>(i); }

private:
    std::array<int, N> m_dimensionExtents; // only required as a member because of the dims functions

};

} // namespace slb
