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
       
// MARK: - HyperBufferPreAllocFlat - manages existing 1D data
template<typename T, int N>
class HyperBufferPreAllocFlat : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using size_type = typename HyperBufferBase<T, N>::size_type;
    using subdim_pointer_type = typename HyperBufferBase<T, N>::subdim_pointer_type;
    using HyperBufferBase<T, N>::STL;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    HyperBufferPreAllocFlat(T* preAllocatedDataFlat, I... i) :
        HyperBufferBase<T, N>(i...),
        m_bufferGeometry(i...),
        m_externalData(preAllocatedDataFlat),
        m_pointers(STL(m_bufferGeometry.getRequiredPointerArraySize()))
    {
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
    
    /** Constructor that takes the extents of the dimensions as a std::array */
    HyperBufferPreAllocFlat(T* preAllocatedDataFlat, std::array<int, N>& dimensionExtents) :
        HyperBufferBase<T, N>(dimensionExtents),
        m_bufferGeometry(dimensionExtents),
        m_externalData(preAllocatedDataFlat),
        m_pointers(STL(m_bufferGeometry.getRequiredPointerArraySize()))
    {
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
    
    // MARK: - common functions not in base class (they differ in return type)
    
    /** Access the raw data - in this case an externally-managed 1D data block */
    T* rawData() const { return m_externalData; }
    T* rawData() { return m_externalData; }
    
    // MARK: operator()
    FOR_N1 T& operator() (size_type i) { return getDataPointer_N1()[i]; }
    FOR_N1 const T& operator() (size_type i) const { return getDataPointer_N1()[i]; }
    FOR_Nx decltype(auto) operator() (size_type dn) { return HyperBufferPreAllocFlat<T, N-1>(*this, dn); }
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) { return HyperBufferPreAllocFlat<T, N-1>(*this, dn).operator()(i...); }

private:
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
    
    const pointer_type getDataPointer_Nx() const override
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
    }
    
    pointer_type getDataPointer_Nx() override
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
    }

    const T* getDataPointer_N1() const override
    {
        return *m_pointers.data();
    }

    T* getDataPointer_N1() override
    {
        return *m_pointers.data();
    }

private:
    friend class HyperBufferPreAllocFlat<T, N-1>;
    friend class HyperBufferPreAllocFlat<T, N+1>;
    
    BufferGeometry<N> m_bufferGeometry;
    T* m_externalData;
    mutable std::vector<T*> m_pointers;
};

// ====================================================================================================================
// MARK: - HyperBuffer - owns its own data
template<typename T, int N>
class HyperBuffer : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using size_type = typename HyperBufferBase<T, N>::size_type;
    using subdim_pointer_type = typename HyperBufferBase<T, N>::subdim_pointer_type;
    using HyperBufferBase<T, N>::STL;

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
    
    // MARK: - common functions not in base class (they differ in return type)
    
    /** Access the raw data - in this case an internally-managed 1D vector */
    const std::vector<T>& rawData() const { return m_data; }
    std::vector<T>& rawData() { return m_data; }
    
    // MARK:  operator()
    FOR_N1 T& operator() (size_type i) { return getDataPointer_N1()[i]; }
    FOR_N1 const T& operator() (size_type i) const { return getDataPointer_N1()[i]; }
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i)
    {
        const int offset = m_bufferGeometry.getDimensionStartOffsetInDataArray(dn);
        T* subDimData = &m_data[offset];
        std::array<int, N-1> subDimExtents = StdArrayOperations::subArray(this->dims());
        // return a view to the desired sub-buffer of this HyperBuffer Object
        return HyperBufferPreAllocFlat<T, N-1>(subDimData, subDimExtents).operator()(i...);
    }
    FOR_Nx decltype(auto) operator() (size_type dn)
    {
        const int offset = m_bufferGeometry.getDimensionStartOffsetInDataArray(dn);
        T* subDimData = &m_data[offset];
        std::array<int, N-1> subDimExtents = StdArrayOperations::subArray(this->dims());
        // return a view to the N-1 sub-buffer of this HyperBuffer Object
        return HyperBufferPreAllocFlat<T, N-1>(subDimData, subDimExtents);
    }

private:
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
    
    const pointer_type getDataPointer_Nx() const override
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
    }
    pointer_type getDataPointer_Nx() override
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
    }

    const T* getDataPointer_N1() const override
    {
        return *m_pointers.data();
    }
    T* getDataPointer_N1() override
    {
        return *m_pointers.data();
    }

private:
    friend class HyperBuffer<T, N-1>;
    friend class HyperBuffer<T, N+1>;
    
    BufferGeometry<N> m_bufferGeometry;
    
    /** All the data (innermost dimension) is stored in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    mutable std::vector<T*> m_pointers;
};


// ====================================================================================================================
// MARK: - HyperBufferPreAlloc - manages existing multi-dimensional data (wrapper)
template<typename T, int N>
class HyperBufferPreAlloc : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using size_type = typename HyperBufferBase<T, N>::size_type;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    HyperBufferPreAlloc(pointer_type preAllocatedData, I... i) :
        HyperBufferBase<T, N>(i...),
        m_externalData(preAllocatedData) {}
    
    // MARK: - common functions not in base class (they differ in return type)
    
    /** Access the raw data - in this case an externally-managed multi-dimensional data block */
    pointer_type rawData() const { return m_externalData; }
    pointer_type rawData() { return m_externalData; }
    
    // MARK: operator()
    FOR_N1 T& operator() (size_type i) { return getDataPointer_N1()[i]; }
    FOR_N1 const T& operator() (size_type i) const { return getDataPointer_N1()[i]; }
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i)
    {
        return HyperBufferPreAlloc<T, N-1>(*this, dn).operator()(i...);
    }
    FOR_Nx decltype(auto) operator() (size_type dn)
    {
        return HyperBufferPreAlloc<T, N-1>(*this, dn);
    }
    
private:
    /** Build a HyperBuffer from an existing N+1 Hyperbuffer */
    HyperBufferPreAlloc(const HyperBufferPreAlloc<T, N+1>& parent, size_type index) :
        HyperBufferBase<T, N>(StdArrayOperations::subArray(parent.dims())),
        m_externalData(parent.rawData()[index]) {}
    
    const pointer_type getDataPointer_Nx() const override
    {
        return m_externalData;
    }
    pointer_type getDataPointer_Nx() override
    {
        return m_externalData;
    }

    const T* getDataPointer_N1() const override
    {
        return reinterpret_cast<T*>(m_externalData);
    }
    T* getDataPointer_N1() override
    {
        return reinterpret_cast<T*>(m_externalData);
    }

private:
    friend class HyperBufferPreAlloc<T, N-1>;
    friend class HyperBufferPreAlloc<T, N+1>;

    pointer_type m_externalData;
};
