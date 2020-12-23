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
    
    // MARK: - common functions not in base class (they differ in return type)
    
    // MARK: operator()
    FOR_N1 T& operator() (size_type i) { return getDataPointer_N1()[i]; }
    FOR_N1 const T& operator() (size_type i) const { return getDataPointer_N1()[i]; }
    
    /** Create sub-buffer by returning a N-1 view (no data ownership) (recursive) */
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) { return HyperBufferPreAllocFlat<T, N-1>(*this, dn).operator()(i...); }
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) const
    {
        const HyperBufferPreAllocFlat<T, N-1> subBuffer(*this, dn); // force use of const version
        return subBuffer.operator()(i...);
    }
    /** Create sub-buffer by returning a N-1 view (no data ownership) (lowest dimension) */
    FOR_Nx decltype(auto) operator() (size_type dn) { return HyperBufferPreAllocFlat<T, N-1>(*this, dn); }
    FOR_Nx decltype(auto) operator() (size_type dn) const
    {
        const HyperBufferPreAllocFlat<T, N-1> subBuffer(*this, dn); // force use of const version
        return subBuffer;
    }

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
    
    const pointer_type getDataPointer_Nx() const override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
    pointer_type getDataPointer_Nx()             override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
    const T* getDataPointer_N1() const           override { return *m_pointers.data(); }
    T* getDataPointer_N1()                       override { return *m_pointers.data(); }

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
    FOR_N1 T& operator() (size_type i) { return getDataPointer_N1()[i]; }
    FOR_N1 const T& operator() (size_type i) const { return getDataPointer_N1()[i]; }
   
    /** Create sub-buffer by returning a N-1 view (no data ownership) in the form of a HyperBufferPreAllocFlat (recursive) */
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) {  return createSubBufferView(dn).operator()(i...); }
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) const {
        const auto subBuffer = createSubBufferView(dn); // force use of const version
        return subBuffer.operator()(i...);
    }
    
    /** Create sub-buffer by returning a N-1 view (no data ownership) in the form of a HyperBufferPreAllocFlat (lowest dimension) */
    FOR_Nx HyperBufferPreAllocFlat<T, N-1> operator() (size_type dn) { return createSubBufferView(dn); }
    FOR_Nx HyperBufferPreAllocFlat<T, N-1> operator() (size_type dn) const
    {
        const auto subBuffer = createSubBufferView(dn); // force use of const version
        return subBuffer;
    }

private:
    const pointer_type getDataPointer_Nx() const override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
    pointer_type getDataPointer_Nx()             override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
    const T* getDataPointer_N1() const           override { return *m_pointers.data(); }
    T* getDataPointer_N1()                       override { return *m_pointers.data(); }
    
    /** Helper function to avoid code duplication in const / non-const operator() */
    HyperBufferPreAllocFlat<T, N-1> createSubBufferView(size_type dn) const
    {
        // NOTE: explicitly cast away the const-ness - need to provide write access to the created HyperBufferPreAllocFlat object
        const int offset = m_bufferGeometry.getDimensionStartOffsetInDataArray(dn);
        T* subDimData = const_cast<T*>(&m_data[offset]);
        std::array<int, N-1> subDimExtents = StdArrayOperations::subArray(this->dims());
        return HyperBufferPreAllocFlat<T, N-1>(subDimData, subDimExtents);
    }

private:
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
    
    // MARK: operator()
    FOR_N1 T& operator() (size_type i) { return getDataPointer_N1()[i]; }
    FOR_N1 const T& operator() (size_type i) const { return getDataPointer_N1()[i]; }
    
    /** Create sub-buffer by returning a N-1 view (no data ownership) (recursive) */
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) { return HyperBufferPreAlloc<T, N-1>(*this, dn).operator()(i...); }
    FOR_Nx_V decltype(auto) operator() (size_type dn, I... i) const
    {
        const HyperBufferPreAlloc<T, N-1> subBuffer(*this, dn); // force use of const version
        return subBuffer.operator()(i...);
    }
    /** Create sub-buffer by returning a N-1 view (no data ownership) (lowest dimension) */
    FOR_Nx decltype(auto) operator() (size_type dn) { return HyperBufferPreAlloc<T, N-1>(*this, dn); }
    FOR_Nx decltype(auto) operator() (size_type dn) const
    {
        const HyperBufferPreAlloc<T, N-1> subBuffer(*this, dn); // force use of const version
        return subBuffer;
    }
    
private:
    /** Build a HyperBuffer from an existing N+1 Hyperbuffer */
    HyperBufferPreAlloc(const HyperBufferPreAlloc<T, N+1>& parent, size_type index) :
        HyperBufferBase<T, N>(StdArrayOperations::subArray(parent.dims())),
        m_externalData(parent.m_externalData[index]) {}
    
    const pointer_type getDataPointer_Nx() const override { return m_externalData; }
    pointer_type getDataPointer_Nx()             override { return m_externalData; }
    const T* getDataPointer_N1() const           override { return reinterpret_cast<T*>(m_externalData); }
    T* getDataPointer_N1()                       override { return reinterpret_cast<T*>(m_externalData); }
    
private:
    friend class HyperBufferPreAlloc<T, N-1>;
    friend class HyperBufferPreAlloc<T, N+1>;

    pointer_type m_externalData;
};

} // namespace slb
