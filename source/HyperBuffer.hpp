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

// MARK: - HyperBufferPreAllocFlat - manages existing 1D data
template<typename T, int N>
class HyperBufferPreAllocFlat : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using const_pointer_type = typename HyperBufferBase<T, N>::const_pointer_type;
    using size_type = typename HyperBufferBase<T, N>::size_type;
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
    
    // MARK: - common functions not in base class (they differ in return type)
    
    // MARK: operator()
    FOR_N1 const T& operator() (size_type i) const { return std::as_const(*this).getDataPointer_N1()[i]; }
    FOR_N1 T& operator() (size_type i) { return getDataPointer_N1()[i]; }
    
    /** Create sub-buffer by returning a N-1 view (no data ownership) (recursive, multiple arguments) */
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) const { return createSubBuffer(*this, dn).operator()(i...); }
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) { return createSubBuffer(*this, dn).operator()(i...); }

    /** Create sub-buffer by returning a N-1 view (no data ownership) (single argument) */
    FOR_Nx decltype(auto) operator() (size_type dn) const { return createSubBuffer(*this, dn); }
    FOR_Nx decltype(auto) operator() (size_type dn) { return createSubBuffer(*this, dn); }

private:
    /** Build a const N-1 HyperBuffer from an existing Hyperbuffer */
    const HyperBufferPreAllocFlat<T, N-1> createSubBuffer(const HyperBufferPreAllocFlat<T, N>& parent, size_type index) const
    {
        ASSERT(index < parent.dims()[0], "Index out of range");
        int offset = parent.m_bufferGeometry.getDataOffsetForHighestOrderDimStart(index);
        return HyperBufferPreAllocFlat<T, N-1>(&parent.m_externalData[offset], StdArrayOperations::subArray(parent.dims()));
    }
    
    /** Build a N-1 HyperBuffer from an existing Hyperbuffer */
    HyperBufferPreAllocFlat<T, N-1> createSubBuffer(const HyperBufferPreAllocFlat<T, N>& parent, size_type index)
    {
        ASSERT(index < parent.dims()[0], "Index out of range");
        int offset = parent.m_bufferGeometry.getDataOffsetForHighestOrderDimStart(index);
        return HyperBufferPreAllocFlat<T, N-1>(&parent.m_externalData[offset], StdArrayOperations::subArray(parent.dims()));
    }
    
    const_pointer_type getDataPointer_Nx() const override { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
    pointer_type getDataPointer_Nx()             override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
    const T* getDataPointer_N1() const           override { return reinterpret_cast<const T*>(*m_pointers.data()); }
    T* getDataPointer_N1()                       override { return *m_pointers.data(); }

private:
    BufferGeometry<N> m_bufferGeometry;
    T* m_externalData;
    std::vector<T*> m_pointers;
};

// ====================================================================================================================
// MARK: - HyperBuffer - owns its own data
template<typename T, int N>
class HyperBuffer : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using const_pointer_type = typename HyperBufferBase<T, N>::const_pointer_type;
    using size_type = typename HyperBufferBase<T, N>::size_type;
    using HyperBufferBase<T, N>::STL;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
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

    // MARK:  operator()
    FOR_N1 const T& operator() (size_type i) const { return getDataPointer_N1()[i]; }
    FOR_N1 T& operator() (size_type i) { return getDataPointer_N1()[i]; }
   
    /** Create sub-buffer by returning a N-1 view (no data ownership) in the form of a HyperBufferPreAllocFlat (recursive, multiple arguments) */
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) const { return createSubBufferView(dn).operator()(i...); }
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) { return createSubBufferView(dn).operator()(i...); }

    
    /** Create sub-buffer by returning a N-1 view (no data ownership) in the form of a HyperBufferPreAllocFlat (single argument) */
    FOR_Nx decltype(auto) operator() (size_type dn) const { return createSubBufferView(dn); }
    FOR_Nx decltype(auto) operator() (size_type dn) { return createSubBufferView(dn); }
    
private:
    /** Build a (non-owning) N-1 HyperBuffer view from an existing N Hyperbuffer */
    const HyperBufferPreAllocFlat<T, N-1> createSubBufferView(size_type dn) const
    {
        const int offset = m_bufferGeometry.getDataOffsetForHighestOrderDimStart(dn);
        // NOTE: explicitly cast away the const-ness - need to provide a non-const pointer to HyperBufferPreAllocFlat ctor, even if we turn it into a const object
        T* subDimData = const_cast<T*>(&m_data[offset]);
        return HyperBufferPreAllocFlat<T, N-1>(subDimData, StdArrayOperations::subArray(this->dims()));
    }
    
    /** Build a (non-owning) N-1 HyperBuffer const view from an existing N Hyperbuffer */
    HyperBufferPreAllocFlat<T, N-1> createSubBufferView(size_type dn)
    {
        const int offset = m_bufferGeometry.getDataOffsetForHighestOrderDimStart(dn);
        return HyperBufferPreAllocFlat<T, N-1>(&m_data[offset], StdArrayOperations::subArray(this->dims()));
    }

    const_pointer_type getDataPointer_Nx() const override { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
    pointer_type getDataPointer_Nx()             override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
    const T* getDataPointer_N1() const           override { return const_cast<const T*>(*m_pointers.data()); }
    T* getDataPointer_N1()                       override { return *m_pointers.data(); }

private:
    BufferGeometry<N> m_bufferGeometry;
    
    /** All the data (innermost dimension) is stored in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    std::vector<T*> m_pointers;
};


// ====================================================================================================================
// MARK: - HyperBufferPreAlloc - manages existing multi-dimensional data (wrapper)
template<typename T, int N>
class HyperBufferPreAlloc : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using const_pointer_type = typename HyperBufferBase<T, N>::const_pointer_type;
    using size_type = typename HyperBufferBase<T, N>::size_type;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    HyperBufferPreAlloc(pointer_type preAllocatedData, I... i) :
        HyperBufferBase<T, N>(i...),
        m_externalData(preAllocatedData) {}
    
    // MARK: - common functions not in base class (they differ in return type)
    
    // MARK: operator()
    FOR_N1 const T& operator() (size_type i) const { return getDataPointer_N1()[i]; }
    FOR_N1 T& operator() (size_type i) { return getDataPointer_N1()[i]; }
    
    /** Create sub-buffer by returning a N-1 view (no data ownership) (recursive, multiple arguments) */
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) const { return createSubBuffer(*this, dn).operator()(i...); }
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) { return createSubBuffer(*this, dn).operator()(i...); }
    
    /** Create sub-buffer by returning a N-1 view (no data ownership) (single argument) */
    FOR_Nx decltype(auto) operator() (size_type dn) const { return createSubBuffer(*this, dn); }
    FOR_Nx decltype(auto) operator() (size_type dn) { return createSubBuffer(*this, dn); }
    
private:
    /** Build a const N-1 HyperBuffer from an existing Hyperbuffer */
    const HyperBufferPreAlloc<T, N-1> createSubBuffer(const HyperBufferPreAlloc<T, N>& parent, size_type index) const
    {
        ASSERT(index < parent.dims()[0], "Index out of range");
        return HyperBufferPreAlloc<T, N-1>(parent.m_externalData[index], StdArrayOperations::subArray(parent.dims()));
    }
    
    /** Build a N-1 HyperBuffer from an existing Hyperbuffer */
    HyperBufferPreAlloc<T, N-1> createSubBuffer(const HyperBufferPreAlloc<T, N>& parent, size_type index)
    {
        ASSERT(index < parent.dims()[0], "Index out of range");
        return HyperBufferPreAlloc<T, N-1>(parent.m_externalData[index], StdArrayOperations::subArray(parent.dims()));
    }

    const_pointer_type getDataPointer_Nx() const override { return const_cast<const_pointer_type>(m_externalData); }
    pointer_type getDataPointer_Nx()             override { return m_externalData; }
    const T* getDataPointer_N1() const           override { return reinterpret_cast<const T*>(m_externalData); }
    T* getDataPointer_N1()                       override { return reinterpret_cast<T*>(m_externalData); }
    
private:
    pointer_type m_externalData;
};

} // namespace slb
