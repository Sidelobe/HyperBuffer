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
    
    /**
     * NOTE: should be simply  `using subdim_pointer_type = typename remove_pointers_from_type<pointer_type, 1>::type;`
     * Otherwise, getTopDimensionData_Nx (which is never called for N=1 anyway) won't compile.
     * We cannot use enable_if on this function because it's virtual!
     * C++17 would allow us to place an `if constexpr`, but in C++14, we have to resort to this hack.
     */
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
    FOR_Nx pointer_type data() { return getTopDimPointer(); }
    FOR_Nx const pointer_type data() const { return getTopDimPointer(); }
    FOR_N1 T* data() { return m_pointers[0]; }
    FOR_N1 const T* data() const { return m_pointers.data(); }

protected:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBufferBase(I... i) :
        m_dimensionExtents{i...},
        m_pointers(STL(getNumberOfPointers(i...)))
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }
    
    /** Constructor that takes the extents of the dimensions as a std::array */
    explicit HyperBufferBase(const std::array<int, N>& dimensionExtents) :
        m_dimensionExtents{dimensionExtents},
        m_pointers(STL(getNumberOfPointers(dimensionExtents))) // at least size 1
    {
        
    }

    constexpr int getNumberOfPointers(const std::array<int, N>& dimensionExtents) const
    {
        return std::max(StdArrayOperations::sumOfCumulativeProductCapped(N-1, dimensionExtents), 1); // at least size 1
    }
    
    template<typename... I>
    constexpr int getNumberOfPointers(I... i) const
    {
        return getNumberOfPointers({i...});
    }
    
    // Helper to make interfacing with STL a bit more readable
    constexpr stl_size_type STL(int i) const { return static_cast<stl_size_type>(i); }

    virtual T& getTopDimensionData_N1(size_type i) = 0;
    virtual subdim_pointer_type getTopDimensionData_Nx(size_type i) = 0;

    /** returns a (multi-dim) pointer to the first entry in the highest-order dimension, e.g. float*** for T=float,N=3 */
    pointer_type getTopDimPointer()
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
    }

protected:
    std::array<int, N> m_dimensionExtents;
    std::vector<T*> m_pointers;
};

