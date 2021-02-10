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
 *  Interface class for all variations of HyperBuffer.
 *  The template parameters set the data type T (e.g. float) and the dimension N
 *
 *  - CRTP (Curiously Recurring Template Pattern) - aka 'static polymorphism' is used to access the
 *    createSubBufferView() functions in the derived classes recursively.
 *
 *  - std::enable_if constructs are required to resolve for different dimensions (no overloaded return types possible)
 *
 *  - virtual functions cannot be used for data(), operator[] and at() because of said std::enable_if and
 *    the deduced return types of decltype(auto) in the return of recursive functions (different for each recursion level)
 *
 */
template<typename T, int N, typename Derived=int> // CRTP with dummy default argument (Derived=int will never be used)
class IHyperBuffer
{
protected:
    using size_type                 = int;
    using pointer_type              = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type        = typename add_const_pointers_to_type<T, N>::type;
    using subdim_pointer_type       = typename remove_pointers_from_type<pointer_type, 1>::type;
    using subdim_const_pointer_type = typename remove_pointers_from_type<const_pointer_type, 1>::type;

public:
    virtual ~IHyperBuffer() = default;
    
    // MARK: dimension extents
    virtual                   int size(int i) const = 0;
    virtual const std::array<int, N>& sizes() const noexcept = 0;

    // MARK: data() -- raw pointer to beginning of underlying storage (pointers or data, depending on dimension)
    FOR_Nx const_pointer_type data() const noexcept { return getDataPointer_Nx(); }
    FOR_Nx       pointer_type data()       noexcept { return getDataPointer_Nx(); }
    FOR_N1           const T* data() const noexcept { return getDataPointer_N1(); }
    FOR_N1                 T* data()       noexcept { return getDataPointer_N1(); }
    
    // MARK: operator[] -- raw data/pointer access; returns pointer N>1, reference for N=1
    FOR_Nx subdim_const_pointer_type operator[] (size_type i) const { return getDataPointer_Nx()[i]; }
    FOR_Nx       subdim_pointer_type operator[] (size_type i)       { return getDataPointer_Nx()[i]; }
    FOR_N1                  const T& operator[] (size_type i) const { return getDataPointer_N1()[i]; }
    FOR_N1                        T& operator[] (size_type i)       { return getDataPointer_N1()[i]; }
    
    // MARK: at() -- data/subbuffer access; returns Derived<T,N-1> instance or data
    FOR_Nx_V decltype(auto) at(size_type dn, I... i) const { return static_cast<const Derived*>(this)->createSubBufferView(dn).at(i...); }
    FOR_Nx_V decltype(auto) at(size_type dn, I... i)       { return static_cast<      Derived*>(this)->createSubBufferView(dn).at(i...); }
    FOR_Nx   decltype(auto) at(size_type dn)         const { return static_cast<const Derived*>(this)->createSubBufferView(dn); }
    FOR_Nx   decltype(auto) at(size_type dn)               { return static_cast<      Derived*>(this)->createSubBufferView(dn); }
    FOR_N1         const T& at(size_type i)          const { return getDataPointer_N1()[i]; }
    FOR_N1               T& at(size_type i)                { return getDataPointer_N1()[i]; }

protected:
    // MARK: Virtual functions to be defined by derived classes
    virtual const_pointer_type getDataPointer_Nx() const noexcept = 0;
    virtual       pointer_type getDataPointer_Nx()       noexcept = 0;
    virtual           const T* getDataPointer_N1() const noexcept = 0;
    virtual                 T* getDataPointer_N1()       noexcept = 0;
    
};

} // namespace slb
