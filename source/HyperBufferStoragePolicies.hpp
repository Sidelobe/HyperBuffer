//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2021 Lorenz Bucher - all rights reserved

#pragma once

#include <array>
#include <vector>

#include "TemplateUtils.hpp"
#include "BufferGeometry.hpp"

namespace slb
{

template<typename T, int N> class HyperBufferViewPolicy; // forward declaration

/**
 *  HyperBuffer is a container for dynamically-allocated N-dimensional datasets. The extents of the dimensions have
 *  to be supplied during construction. Memory for the pointers and the data is allocated separately (with  ownership).
 *
 *  - Template parameters: T=data type (e.g. float),  N=dimension (e.g. 3)
 *
 *  - Guarantees: Dynamic memory allocation only during construction and when calling subView()
 */
template<typename T, int N>
class HyperBufferOwningPolicy
{
    using size_type                 = int;
    using pointer_type              = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type        = typename add_const_pointers_to_type<T, N>::type;

public:
    using SubBufferPolicy = HyperBufferViewPolicy<T, N-1>;
    
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBufferOwningPolicy(I... i) :
        m_bufferGeometry(i...),
        m_data(m_bufferGeometry.getRequiredDataArraySize()),
        m_pointers(m_bufferGeometry.getRequiredPointerArraySize())
    {
        ASSERT(CompiletimeMath::areAllPositive(i...), "Invalid Dimension extents");
        m_bufferGeometry.hookupPointerArrayToData(m_data.data(), m_pointers.data());
    }
    
    /** @return a pointer to a subdimension of the data */
    T* getSubDimData(size_type index) const
    {
        ASSERT(index < this->size(0), "Index out of range");
        const int offset = m_bufferGeometry.getDataArrayOffsetForHighestOrderSubDim(index);
        return const_cast<T*>(&m_data[offset]); // TODO: comment const-cast
    }
    
    const T* getRawData() const { return m_data.data(); }

    int size(int i) const { ASSERT(i < N); return m_bufferGeometry.getDimensionExtents()[i]; }
    const std::array<int, N>& sizes() const noexcept { return m_bufferGeometry.getDimensionExtents(); }

    const_pointer_type getDataPointer_Nx() const noexcept { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
          pointer_type getDataPointer_Nx()       noexcept { return reinterpret_cast<pointer_type>(m_pointers.data()); }
              const T* getDataPointer_N1() const noexcept { return *m_pointers.data(); }
                    T* getDataPointer_N1()       noexcept { return *m_pointers.data(); }

private:
    /** Handles the geometry (organization) of the data memory, enabling multi-dimensional access to it */
    BufferGeometry<N> m_bufferGeometry;
    
    /** All the data (innermost dimension) is stored in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    std::vector<T*> m_pointers;
};


// ====================================================================================================================
/**
 *  A wrapper for existing HyperBuffer data, giving it the same API, but without data ownership. The extents of the
 *  dimensions have to be supplied during construction. The pre-allocated data is expected to be in a
 *  flat (one-dimensional), contiguous memory block. Pointer memory is allocated during construction and, unlike
 *  data memory, is owned by a given instance of this class.
 *
 *  - Template parameters: T=data type (e.g. float),  N=dimension (e.g. 3)
 *
 *  - Guarantees: Dynamic memory allocation only during construction and when calling .at()
 */
template<typename T, int N>
class HyperBufferViewPolicy
{
    using size_type                 = int;
    using pointer_type              = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type        = typename add_const_pointers_to_type<T, N>::type;
    
public:
    using SubBufferPolicy = HyperBufferViewPolicy<T, N-1>;
    
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    HyperBufferViewPolicy(T* preAllocatedDataFlat, I... i) :
        m_bufferGeometry(i...),
        m_externalData(preAllocatedDataFlat),
        m_pointers(m_bufferGeometry.getRequiredPointerArraySize())
    {
        ASSERT(CompiletimeMath::areAllPositive(i...), "Invalid Dimension extents");
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
    

    /** Constructor that takes an existing (owning) Buffer and creates a (non-owning) View from it */
    explicit HyperBufferViewPolicy(const HyperBufferOwningPolicy<T, N>& owningBufferPolicy) :
        m_bufferGeometry(owningBufferPolicy.sizes()),
        m_externalData(const_cast<T*>(owningBufferPolicy.getRawData())), // TODO: const_cast here... :-(
        m_pointers(m_bufferGeometry.getRequiredPointerArraySize())
    {
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
    
    /** @return a pointer to a subdimension of the data */
    T* getSubDimData(size_type index) const
    {
        const int offset = m_bufferGeometry.getDataArrayOffsetForHighestOrderSubDim(index);
        return &m_externalData[offset];
    }
    
    int size(int i) const { ASSERT(i < N); return m_bufferGeometry.getDimensionExtents()[i]; }
    const std::array<int, N>& sizes() const noexcept { return m_bufferGeometry.getDimensionExtents(); }

    const_pointer_type  getDataPointer_Nx() const noexcept { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
           pointer_type getDataPointer_Nx()       noexcept { return reinterpret_cast<pointer_type>(m_pointers.data()); }
               const T* getDataPointer_N1() const noexcept { return *m_pointers.data(); }
                     T* getDataPointer_N1()       noexcept { return *m_pointers.data(); }
    
private:
    /** Handles the geometry (organization) of the data memory, enabling multi-dimensional access to it */
    BufferGeometry<N> m_bufferGeometry;
    
    /** Pointer to the externally-allocated data memory */
    T* m_externalData;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    std::vector<T*> m_pointers;
};

// ====================================================================================================================
/**
 *  A wrapper for existing multi-dimensional data (e.g. float**), giving it the same API as HyperBuffer. The extents
 *  of the dimensions have to be supplied during construction. Both pointer and data memory are stored externally
 *  (this class has no ownership).
 *
 *  - Template parameters: T=data type (e.g. float),  N=dimension (e.g. 3)
 *
 *  - Guarantees: Does not allocate any memory dynamically.
 */
template<typename T, int N>
class HyperBufferViewMDPolicy
{
    using size_type                 = int;
    using pointer_type              = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type        = typename add_const_pointers_to_type<T, N>::type;
    using subdim_pointer_type       = typename remove_pointers_from_type<pointer_type, 1>::type;
    
public:
    using SubBufferPolicy = HyperBufferViewMDPolicy<T, N-1>;
    
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    HyperBufferViewMDPolicy(pointer_type preAllocatedData, I... i) :
        m_dimensionExtents{static_cast<int>(i)...},
        m_externalData(preAllocatedData)
    {
        ASSERT(CompiletimeMath::areAllPositive(i...), "Invalid Dimension extents");
    }
            
    /** Constructor that takes the extents of the dimensions as a std::array */
    HyperBufferViewMDPolicy(pointer_type preAllocatedData, std::array<int, N> dimensionExtents) :
        m_dimensionExtents(dimensionExtents),
        m_externalData(preAllocatedData)
    {
        ASSERT(CompiletimeMath::areAllPositive(m_dimensionExtents), "Invalid Dimension extents");
    }
    
    /** Constructor that takes the extents of the dimensions as a std::vector */
    HyperBufferViewMDPolicy(pointer_type preAllocatedData, std::vector<int> dimensionExtentsVector) :
        m_externalData(preAllocatedData)
    {
        ASSERT(dimensionExtentsVector.size() == N, "Incorrect number of dimension extents");
        std::copy(dimensionExtentsVector.begin(), dimensionExtentsVector.end(), m_dimensionExtents.begin());
        ASSERT(CompiletimeMath::areAllPositive(m_dimensionExtents), "Invalid Dimension extents");
    }
    
    /** @return a pointer to a subdimension of the data */
    subdim_pointer_type getSubDimData(size_type index) const
    {
        return m_externalData[index];
    }

    int size(int i) const { ASSERT(i < N); return m_dimensionExtents[i]; }
    const std::array<int, N>& sizes() const noexcept { return m_dimensionExtents; }

    const_pointer_type getDataPointer_Nx() const noexcept { return m_externalData; }
         pointer_type  getDataPointer_Nx()       noexcept { return m_externalData; }
              const T* getDataPointer_N1() const noexcept { return reinterpret_cast<const T*>(m_externalData); }
                    T* getDataPointer_N1()       noexcept { return reinterpret_cast<T*>(m_externalData); }
    
private:
    std::array<int, N> m_dimensionExtents;
    
    /** Pointer to the externally-allocated multi-dimensional data memory */
    pointer_type m_externalData;
};


} // namespace slb
