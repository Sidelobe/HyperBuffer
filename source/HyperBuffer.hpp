//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <array>
#include <vector>

#include "IHyperBuffer.hpp"
#include "TemplateUtils.hpp"
#include "BufferGeometry.hpp"

#pragma once

// MARK: - HyperBufferOwning
template<typename T, int N>
class HyperBuffer : public IHyperBuffer<T, N>
{
    using size_type = typename IHyperBuffer<T, N>::size_type;
    using pointer_type = typename IHyperBuffer<T, N>::pointer_type;
    using subdim_pointer_type = typename remove_pointers_from_type<pointer_type, 1>::type;

public:
    template<typename... I>
    explicit HyperBuffer(I... i) :
        m_bufferGeometry(i...),
        m_data(m_bufferGeometry.getRequiredDataArraySize()),
        m_pointers(m_bufferGeometry.getRequiredDataArraySize())
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
        
        m_bufferGeometry.hookupPointerArrayToData(m_data.data(), m_pointers.data());
    }
    
    // MARK: - operator[]
    template<int M=N, std::enable_if_t<(M>1), int> = 0>
    subdim_pointer_type operator[] (size_type i)
    {
        // TODO: should we return a reference here --??
        // TODO: return sub-buffer? something like: HyperBuffer<T, N-1>(*this, i)
        assert(i < m_bufferGeometry.getDimensionExtents()[0] && "index out range");
        int offset = m_bufferGeometry.template getOffsetInPointerArray<0>(i);
        return reinterpret_cast<subdim_pointer_type>(m_pointers[offset]);
    }
    
    template<int M=N, std::enable_if_t<(M>1), int> = 0>
    const subdim_pointer_type operator[] (size_type i) const { return this->operator[](i); }
        
    template<int M=N, std::enable_if_t<(M<=1), int> = 0> T& operator[] (size_type i) { return m_data[i]; }
    template<int M=N, std::enable_if_t<(M<=1), int> = 0> const T& operator[] (size_type i) const { return m_data[i]; }

    // TODO: operator(varArg) -- returns a sub-buffer or value, depending on number of arguments
    //    template<int... I>
    //    decltype(auto) operator() (size_type i...) = 0;
    
    // MARK: dimension extents
    int dim(int i) const { return m_bufferGeometry.getDimensionExtents()[i]; }
    const int* dims() const { return m_bufferGeometry.getDimensionExtentsPointer(); }

    // MARK: data()
    template<int M=N, std::enable_if_t<(M>1), int> = 0> pointer_type data() { return getTopDimPointer(); }
    template<int M=N, std::enable_if_t<(M>1), int> = 0> const pointer_type data() const { return getTopDimPointer(); }
    template<int M=N, std::enable_if_t<(M==1), int> = 0> T* data() { return m_pointers[0]; }
    template<int M=N, std::enable_if_t<(M==1), int> = 0> const T* data() const { return m_pointers.data(); }
    
private:
    /** returns a (multi-dim) pointer to the first entry in the highest-order dimension, e.g. float*** for T=float,N=3 */
    pointer_type getTopDimPointer()
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
    }

private:
    BufferGeometry<N> m_bufferGeometry;
    
    /** we store the data in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    std::vector<T*> m_pointers; // TODO: move to base class
};


// MARK: - HyperBufferPreAlloc
/** Construct from pre-allocated, multi-dimensional data */
template<typename T, int N>
class HyperBufferPreAlloc : public IHyperBuffer<T, N>
{
    using size_type = typename IHyperBuffer<T, N>::size_type;
    using pointer_type = typename IHyperBuffer<T, N>::pointer_type;
    
public:
    template<typename... I>
    explicit HyperBufferPreAlloc(pointer_type preAllocatedData, I... i) :
        m_dataPointers(preAllocatedData)
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }
    
    pointer_type getDataPointers() const
    {
        return m_dataPointers;
    }
    
private:
    pointer_type m_dataPointers;
};

// MARK: - HyperBufferPreAllocFlat
/** Construct from pre-allocated, flat (1D) data */
template<typename T, int N>
class HyperBufferPreAllocFlat : public IHyperBuffer<T, N>
{
    using size_type = typename IHyperBuffer<T, N>::size_type;
    using pointer_type = typename IHyperBuffer<T, N>::pointer_type;
    
public:
//    explicit HyperBufferPreAllocFlat(T* preAllocatedDataFlat, I... i)
//    {
//        // TODO:
//    }
    
};
