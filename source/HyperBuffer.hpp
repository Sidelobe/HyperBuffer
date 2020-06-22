//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <array>
#include <vector>
#include <cstring> // for memcpy

#include "HyperBufferBase.hpp"
#include "TemplateUtils.hpp"
#include "BufferGeometry.hpp"
            
// MARK: - HyperBuffer - owns the data
template<typename T, int N>
class HyperBuffer : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using size_type = typename HyperBufferBase<T, N>::size_type;
    using HyperBufferBase<T, N>::STL;
    
    friend class HyperBuffer<T, N-1>;
    
public:
    template<typename... I>
    explicit HyperBuffer(I... i) :
        HyperBufferBase<T, N>(i...),
        m_bufferGeometry(i...),
        m_data(STL(m_bufferGeometry.getRequiredDataArraySize())),
        m_pointers(STL(m_bufferGeometry.getRequiredPointerArraySize()))
    {
        m_bufferGeometry.hookupPointerArrayToData(m_data.data(), m_pointers.data());
    }
    
    /** Build a HyperBuffer from an existing N+1 Hyperbuffer */
    HyperBuffer(const HyperBuffer<T, N+1>& parent, size_type index) :
        HyperBufferBase<T, N>(StdArrayOperations::subArray(parent.dims())),
        m_bufferGeometry(StdArrayOperations::subArray(parent.dims())),
        m_data(STL(m_bufferGeometry.getRequiredDataArraySize())),
        m_pointers(STL(m_bufferGeometry.getRequiredPointerArraySize()))
    {
        m_bufferGeometry.hookupPointerArrayToData(m_data.data(), m_pointers.data());
        
        // copy data of selected sub-tree
        int subTreeOffset = parent.m_bufferGeometry.getDimensionStartOffsetInDataArray(index);
        std::copy(parent.rawData().data() + subTreeOffset, parent.rawData().data() + subTreeOffset + m_bufferGeometry.getRequiredDataArraySize(), m_data.begin());
    }
    
    /** Access the raw data - in this case an internally-managed 1D vector */
    const std::vector<T>& rawData() const { return m_data; }
    std::vector<T>& rawData() { return m_data; }

private:
    pointer_type getDataPointer_Nx() const override
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
    }
    
    T* getDataPointer_N1() const override
    {
        return *m_pointers.data();
    }
    
private:
    BufferGeometry<N> m_bufferGeometry;
    
    /** All the data (innermost dimension) is stored in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    mutable std::vector<T*> m_pointers;
};


// MARK: - HyperBufferPreAllocFlat - manages existing 1D data
template<typename T, int N>
class HyperBufferPreAllocFlat : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using size_type = typename HyperBufferBase<T, N>::size_type;
    using HyperBufferBase<T, N>::STL;
    
    friend class HyperBufferPreAllocFlat<T, N-1>;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBufferPreAllocFlat(T* preAllocatedDataFlat, I... i) :
        HyperBufferBase<T, N>(i...),
        m_bufferGeometry(i...),
        m_externalData(preAllocatedDataFlat),
        m_pointers(STL(m_bufferGeometry.getRequiredPointerArraySize()))
    {
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
    
    /** Build a HyperBuffer from an existing N+1 Hyperbuffer */
    HyperBufferPreAllocFlat(const HyperBufferPreAllocFlat<T, N+1>& parent, size_type index) :
        HyperBufferBase<T, N>(StdArrayOperations::subArray(parent.dims())),
        m_bufferGeometry(StdArrayOperations::subArray(parent.dims())),
        m_pointers(STL(m_bufferGeometry.getRequiredPointerArraySize()))
    {
        int offset = parent.m_bufferGeometry.getDimensionStartOffsetInDataArray(index);
        m_externalData = &parent.m_externalData[offset];
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
    
    template<typename... I>
    decltype(auto) operator() (int dn, I... i)
    {
        // TODO: get rid of this constexpr if to reach C++14 compatibility
        if constexpr(N == 1) {
            return getDataPointer_N1()[dn];
        } else if constexpr(sizeof...(i) == 0) {
            return HyperBufferPreAllocFlat<T, N-1>(*this, dn);
        } else {
            return HyperBufferPreAllocFlat<T, N-1>(*this, dn).operator()(i...);
        }
    }

    /** Access the raw data - in this case an externally-managed 1D data block */
    T* rawData() const { return m_externalData; }
    T* rawData() { return m_externalData; }
    
private:
    pointer_type getDataPointer_Nx() const override
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
    }
    
    T* getDataPointer_N1() const override
    {
        return *m_pointers.data();
    }
    
private:
    BufferGeometry<N> m_bufferGeometry;
    T* m_externalData;
    mutable std::vector<T*> m_pointers;
};



// MARK: - HyperBufferPreAlloc - manages existing multi-dimensional data (wrapper)
template<typename T, int N>
class HyperBufferPreAlloc : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    
public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBufferPreAlloc(pointer_type preAllocatedData, I... i) :
        HyperBufferBase<T, N>(i...),
        m_externalData(preAllocatedData) {}
    
    /** Access the raw data - in this case an externally-managed multi-dimensional data block */
    pointer_type rawData() { return m_externalData; }
    
private:
    pointer_type getDataPointer_Nx() const override
    {
        return m_externalData;
    }
    
    T* getDataPointer_N1() const override
    {
        return reinterpret_cast<T*>(m_externalData);
    }
    
private:
    pointer_type m_externalData;
};
