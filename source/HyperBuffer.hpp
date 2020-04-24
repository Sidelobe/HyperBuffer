//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <array>
#include <vector>

#include "IHyperBuffer.hpp"
#include "TemplateUtils.hpp"
#include "BufferGeometry.hpp"

// MARK: - HyperBufferOwning
template<typename T, int N>
class HyperBuffer : public IHyperBuffer<T, N>
{
    using size_type = typename IHyperBuffer<T, N>::size_type;
    using pointer_type = typename IHyperBuffer<T, N>::pointer_type;
    using subdim_pointer_type = typename IHyperBuffer<T, N>::subdim_pointer_type;

public:
    template<typename... I>
    explicit HyperBuffer(I... i) :
        IHyperBuffer<T, N>(i...),
        m_bufferGeometry(i...),
        m_data(m_bufferGeometry.getRequiredDataArraySize())
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
        
        m_bufferGeometry.hookupPointerArrayToData(m_data.data(), IHyperBuffer<T, N>::m_pointers.data());
    }
    
private:
    T& getTopDimensionData_N1(size_type i) override
    {
        return m_data[i];
    }
    
    subdim_pointer_type getTopDimensionData_Nx(size_type i) override
    {
        // TODO: should we return a reference here --??
        // TODO: return sub-buffer? something like: HyperBuffer<T, N-1>(*this, i)
        
        // TODO: how can I get this not to compile for N=1 in a virtual function?
        if constexpr (N>1) {
            assert(i < m_bufferGeometry.getDimensionExtents()[0] && "index out range");
            int offset = m_bufferGeometry.template getOffsetInPointerArray<0>(i);
            return reinterpret_cast<subdim_pointer_type>(IHyperBuffer<T, N>::m_pointers[offset]);
        } else {
            return 0;
        }
    }

private:
    BufferGeometry<N> m_bufferGeometry;
    
    /** we store the data in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
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
