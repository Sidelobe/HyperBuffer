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
    int dim(int i) const { return m_dimensionExtents[STL(i)]; }
    const int* dims() const { return m_dimensionExtents.data(); }

    // MARK: - operator[]
    FOR_Nx subdim_pointer_type operator[] (size_type i) { return getDataPointer_Nx()[i]; }
    FOR_Nx const subdim_pointer_type operator[] (size_type i) const { return getDataPointer_Nx()[i]; }
    FOR_N1 T& operator[] (size_type i) { return getDataPointer_N1()[i]; }
    FOR_N1 const T& operator[] (size_type i) const { return getDataPointer_N1()[i]; }

    // TODO: operator(varArg) -- returns a sub-buffer or value, depending on number of arguments
    //    template<int... I>
    //    decltype(auto) operator() (size_type i...) = 0;

    // MARK: data()
    // NOTE: We cannot make these virtual functions because of the differente return types required.
    // decltype(auto) is not allowed for virtual functions, so I chose an enable_if construct for selective compilation
    FOR_Nx pointer_type data() { return getDataPointer_Nx(); }
    FOR_Nx const pointer_type data() const { return getDataPointer_Nx(); }
    FOR_N1 T* data() { return getDataPointer_N1(); }
    FOR_N1 const T* data() const { return getDataPointer_N1(); }

protected:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBufferBase(I... i) :
        m_dimensionExtents{static_cast<int>(i)...}
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }
    
    /** Constructor that takes the extents of the dimensions as a std::array */
    explicit HyperBufferBase(const std::array<int, N>& dimensionExtents) : m_dimensionExtents{dimensionExtents} {}

    // MARK: Virtual functions to be defined by derived classes
    virtual pointer_type getDataPointer_Nx() = 0;
    virtual T* getDataPointer_N1() = 0;
    
     // MARK: Helpers
    static constexpr int getNumberOfPointers(const std::array<int, N>& dimensionExtents)
    {
        return std::max(StdArrayOperations::sumOfCumulativeProductCapped(N-1, dimensionExtents), 1); // at least size 1
    }

    template<typename... I>
    static constexpr int getNumberOfPointers(I... i) { return getNumberOfPointers({static_cast<int>(i)...}); }
    
    // Helper to make interfacing with STL a bit more readable
    constexpr stl_size_type STL(int i) const { return static_cast<stl_size_type>(i); }

protected:
    std::array<int, N> m_dimensionExtents; // only required by the dims functions

};

