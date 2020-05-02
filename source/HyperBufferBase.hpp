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
    
    /**
     * NOTE: should be simply:
     *      using pointer_type = typename add_pointers_to_type<T, N>::type;
     *      using subdim_pointer_type = typename remove_pointers_from_type<pointer_type, 1>::type;
     * But without the std::conditional, getDataPointer_Nx and getTopDimensionData_Nx (which are never called
     * for N=1 anyway) won't compile for N=1. We cannot use enable_if on this function because they're virtual!
     * C++17 would allow us to place an `if constexpr`, but in C++14, we have to resort to this hack.
     */
    using pointer_type = std::conditional_t<(N==1), T*, typename add_pointers_to_type<T, N>::type>;
    using subdim_pointer_type = std::conditional_t<(N==1), T*, typename remove_pointers_from_type<pointer_type, 1>::type>;

    // Helper to make interfacing with STL a bit more readable
    using stl_size_type = typename std::vector<T*>::size_type;
    
public:
    virtual ~HyperBufferBase() = default;
    
    // MARK: dimension extents
    int dim(int i) const { return m_dimensionExtents[STL(i)]; }
    const int* dims() const { return m_dimensionExtents.data(); }

    // MARK: - operator[]
    FOR_Nx subdim_pointer_type operator[] (size_type i) { return getTopDimensionData_Nx(i); }
    FOR_Nx const subdim_pointer_type operator[] (size_type i) const { return getTopDimensionData_Nx(i); }
    FOR_N1 T& operator[] (size_type i) { return getTopDimensionData_N1(i); }
    FOR_N1 const T& operator[] (size_type i) const { return getTopDimensionData_N1(i); }

    // TODO: operator(varArg) -- returns a sub-buffer or value, depending on number of arguments
    //    template<int... I>
    //    decltype(auto) operator() (size_type i...) = 0;

    // MARK: data()
    // NOTE: We cannot make these virtual functions because of the differente return types required.
    // decltype(auto) is not allowed for virtual functions, so I chose an enable_if construct for selective compilation
    FOR_Nx pointer_type data() { return getDataPointer_Nx(); }
    FOR_Nx const pointer_type data() const { return getDataPointer_Nx(); }
    FOR_N1 T* data() { return &getTopDimensionData_N1(0); }
    FOR_N1 const T* data() const { return &getTopDimensionData_N1(0); }

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

    static constexpr int getNumberOfPointers(const std::array<int, N>& dimensionExtents)
    {
        return std::max(StdArrayOperations::sumOfCumulativeProductCapped(N-1, dimensionExtents), 1); // at least size 1
    }
    
    template<typename... I>
    static constexpr int getNumberOfPointers(I... i)
    {
        return getNumberOfPointers({static_cast<int>(i)...});
    }
    
    // Helper to make interfacing with STL a bit more readable
    constexpr stl_size_type STL(int i) const { return static_cast<stl_size_type>(i); }

    virtual T& getTopDimensionData_N1(size_type i) = 0;
    virtual subdim_pointer_type getTopDimensionData_Nx(size_type i) = 0;
    virtual pointer_type getDataPointer_Nx() = 0;

protected:
    std::array<int, N> m_dimensionExtents; // only required by the dims functions

};

