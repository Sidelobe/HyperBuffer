//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <array>
#include <vector>

#include "HyperBufferBase.hpp"
#include "TemplateUtils.hpp"
#include "BufferGeometry.hpp"

namespace slb
{

template<typename T, int N> class HyperBuffer; // forward-declare

// MARK: - HyperBufferPreAllocFlat - manages existing 1D data
template<typename T, int N>
class HyperBufferPreAllocFlat : public HyperBufferBase<T, N, HyperBufferPreAllocFlat<T, N>>
{
    using pointer_type = typename HyperBufferBase<T, N, HyperBufferPreAllocFlat>::pointer_type;
    using const_pointer_type = typename HyperBufferBase<T, N, HyperBufferPreAllocFlat>::const_pointer_type;
    using size_type = typename HyperBufferBase<T, N, HyperBufferPreAllocFlat>::size_type;
    using HyperBufferBase<T, N, HyperBufferPreAllocFlat>::STL;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    HyperBufferPreAllocFlat(T* preAllocatedDataFlat, I... i) :
        HyperBufferBase<T, N, HyperBufferPreAllocFlat<T, N>>(i...),
        m_bufferGeometry(i...),
        m_externalData(preAllocatedDataFlat),
        m_pointers(STL(m_bufferGeometry.getRequiredPointerArraySize()))
    {
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
    
private:
    /** Build a const N-1 HyperBuffer view to this Hyperbuffer's data */
    const HyperBufferPreAllocFlat<T, N-1> createSubBufferView(size_type index) const
    {
        ASSERT(index < this->dim(0), "Index out of range");
        int offset = m_bufferGeometry.getDataArrayOffsetForHighestOrderSubDim(index);
        return HyperBufferPreAllocFlat<T, N-1>(&m_externalData[offset], StdArrayOperations::shaveOffFirstElement(this->dims()));
    }
    
    /** Build a N-1 HyperBuffer view to this Hyperbuffer's data */
    HyperBufferPreAllocFlat<T, N-1> createSubBufferView(size_type index)
    {
        return std::as_const(*this).createSubBufferView(index);
    }
    
    const_pointer_type getDataPointer_Nx() const override { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
    pointer_type getDataPointer_Nx()             override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
    const T* getDataPointer_N1() const           override { return *m_pointers.data(); }
    T* getDataPointer_N1()                       override { return *m_pointers.data(); }

private:
    friend HyperBufferBase<T, N, HyperBufferPreAllocFlat<T, N>>;
    friend HyperBufferPreAllocFlat<T, N+1>;
    friend HyperBuffer<T, N+1>;

    BufferGeometry<N> m_bufferGeometry;
    T* m_externalData;
    std::vector<T*> m_pointers;
};

// ====================================================================================================================
// MARK: - HyperBuffer - owns its own data
template<typename T, int N>
class HyperBuffer : public HyperBufferBase<T, N, HyperBuffer<T, N>>
{
    using pointer_type = typename HyperBufferBase<T, N, HyperBuffer<T, N>>::pointer_type;
    using const_pointer_type = typename HyperBufferBase<T, N, HyperBuffer<T, N>>::const_pointer_type;
    using size_type = typename HyperBufferBase<T, N, HyperBuffer<T, N>>::size_type;
    using HyperBufferBase<T, N, HyperBuffer<T, N>>::STL;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBuffer(I... i) :
        HyperBufferBase<T, N,HyperBuffer<T, N>>(i...),
        m_bufferGeometry(i...),
        m_data(STL(m_bufferGeometry.getRequiredDataArraySize())),
        m_pointers(STL(m_bufferGeometry.getRequiredPointerArraySize()))
    {
        m_bufferGeometry.hookupPointerArrayToData(m_data.data(), m_pointers.data());
    }
    
    // MARK: - common functions not in base class (they differ in return type)

private:
    /** Build a (non-owning) N-1 HyperBuffer view to this Hyperbuffer's data */
    const HyperBufferPreAllocFlat<T, N-1> createSubBufferView(size_type index) const
    {
        ASSERT(index < this->dim(0), "Index out of range");
        const int offset = m_bufferGeometry.getDataArrayOffsetForHighestOrderSubDim(index);
        // NOTE: explicitly cast away the const-ness - need to provide a non-const pointer to HyperBufferPreAllocFlat ctor, even if we turn it into a const object
        T* subDimData = const_cast<T*>(&m_data[offset]);
        return HyperBufferPreAllocFlat<T, N-1>(subDimData, StdArrayOperations::shaveOffFirstElement(this->dims()));
    }
    
    /** Build a (non-owning) N-1 HyperBuffer const view to this Hyperbuffer's data */
    HyperBufferPreAllocFlat<T, N-1> createSubBufferView(size_type index)
    {
        return std::as_const(*this).createSubBufferView(index);
    }

    const_pointer_type getDataPointer_Nx() const override { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
    pointer_type getDataPointer_Nx()             override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
    const T* getDataPointer_N1() const           override { return *m_pointers.data(); }
    T* getDataPointer_N1()                       override { return *m_pointers.data(); }

private:
    friend HyperBufferBase<T, N, HyperBuffer<T, N>>;

    BufferGeometry<N> m_bufferGeometry;
    
    /** All the data (innermost dimension) is stored in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    std::vector<T*> m_pointers;
};

// ====================================================================================================================
// MARK: - HyperBufferPreAlloc - manages existing multi-dimensional data (wrapper)
template<typename T, int N>
class HyperBufferPreAlloc : public HyperBufferBase<T, N, HyperBufferPreAlloc<T, N>>
{
    using pointer_type = typename HyperBufferBase<T, N, HyperBufferPreAlloc<T, N>>::pointer_type;
    using const_pointer_type = typename HyperBufferBase<T, N, HyperBufferPreAlloc<T, N>>::const_pointer_type;
    using size_type = typename HyperBufferBase<T, N, HyperBufferPreAlloc<T, N>>::size_type;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    HyperBufferPreAlloc(pointer_type preAllocatedData, I... i) :
        HyperBufferBase<T, N, HyperBufferPreAlloc<T, N>>(i...),
        m_externalData(preAllocatedData) {}
    
private:
    /** Build a const N-1 HyperBuffer view to this Hyperbuffer's data */
    const HyperBufferPreAlloc<T, N-1> createSubBufferView(size_type index) const
    {
        ASSERT(index < this->dim(0), "Index out of range");
        return HyperBufferPreAlloc<T, N-1>(m_externalData[index], StdArrayOperations::shaveOffFirstElement(this->dims()));
    }
    
    /** Build a N-1 HyperBuffer view to this Hyperbuffer's data */
    HyperBufferPreAlloc<T, N-1> createSubBufferView(size_type index)
    {
        return std::as_const(*this).createSubBufferView(index);
    }

    const_pointer_type getDataPointer_Nx() const override { return m_externalData; }
    pointer_type getDataPointer_Nx()             override { return m_externalData; }
    const T* getDataPointer_N1() const           override { return reinterpret_cast<const T*>(m_externalData); }
    T* getDataPointer_N1()                       override { return reinterpret_cast<T*>(m_externalData); }
    
private:
    friend HyperBufferBase<T, N, HyperBufferPreAlloc<T, N>>;
    friend HyperBufferPreAlloc<T, N+1>;

    pointer_type m_externalData;
};

} // namespace slb
