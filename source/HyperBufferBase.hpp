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

public:
    // MARK: dimension extents
    int dim(int i) const { return m_dimensionExtents[i]; }
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
    template<typename... I>
    explicit HyperBufferBase(I... i) :
        m_dimensionExtents{i...},
        m_pointers(std::max(VarArgOperations::sumOfCumulativeProductCapped(N-1, i...), 1))
    {}

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

