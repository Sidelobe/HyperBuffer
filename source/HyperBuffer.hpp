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

namespace slb
{

template<typename T, int N> class HyperBufferView; // forward declaration


// MARK: - HyperBuffer - owns its own data
template<typename T, int N>
class HyperBuffer : public IHyperBuffer<T, N, HyperBuffer<T, N>>
{
    using typename IHyperBuffer<T, N, HyperBuffer<T, N>>::size_type;
    using typename IHyperBuffer<T, N, HyperBuffer<T, N>>::pointer_type;
    using typename IHyperBuffer<T, N, HyperBuffer<T, N>>::const_pointer_type;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBuffer(I... i) :
        m_bufferGeometry(i...),
        m_data(m_bufferGeometry.getRequiredDataArraySize()),
        m_pointers(m_bufferGeometry.getRequiredPointerArraySize())
    {
        m_bufferGeometry.hookupPointerArrayToData(m_data.data(), m_pointers.data());
    }
    
    int size(int i) const override { ASSERT(i < N); return m_bufferGeometry.getDimensionExtents()[i]; }
    const std::array<int, N>& sizes() const override { return m_bufferGeometry.getDimensionExtents(); }
    
private:
    /** Build a (non-owning) N-1 HyperBuffer view to this Hyperbuffer's data */
    const HyperBufferView<T, N-1> createSubBufferView(size_type index) const
    {
        ASSERT(index < this->size(0), "Index out of range");
        const int offset = m_bufferGeometry.getDataArrayOffsetForHighestOrderSubDim(index);
        // NOTE: explicitly cast away the const-ness - need to provide a non-const pointer to the
        // HyperBufferView ctor, even if we turn it into a const object upon return
        T* subDimData = const_cast<T*>(&m_data[offset]);
        return HyperBufferView<T, N-1>(subDimData, StdArrayOperations::shaveOffFirstElement(this->sizes()));
    }
    
    /** Build a (non-owning) N-1 HyperBuffer const view to this Hyperbuffer's data */
    HyperBufferView<T, N-1> createSubBufferView(size_type index)
    {
        return std::as_const(*this).createSubBufferView(index);
    }

    const_pointer_type getDataPointer_Nx() const override { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
          pointer_type getDataPointer_Nx()       override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
              const T* getDataPointer_N1() const override { return *m_pointers.data(); }
                    T* getDataPointer_N1()       override { return *m_pointers.data(); }

private:
    friend IHyperBuffer<T, N, HyperBuffer<T, N>>;

    BufferGeometry<N> m_bufferGeometry;
    
    /** All the data (innermost dimension) is stored in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    std::vector<T*> m_pointers;
};


// ====================================================================================================================
// MARK: - HyperBufferView - owns only pointers to manage existing multidimensional data stored in a 1D buffer
template<typename T, int N>
class HyperBufferView : public IHyperBuffer<T, N, HyperBufferView<T, N>>
{
    using typename IHyperBuffer<T, N, HyperBufferView>::pointer_type;
    using typename IHyperBuffer<T, N, HyperBufferView>::const_pointer_type;
    using typename IHyperBuffer<T, N, HyperBufferView>::size_type;
    
public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    HyperBufferView(T* preAllocatedDataFlat, I... i) :
    m_bufferGeometry(i...),
    m_externalData(preAllocatedDataFlat),
    m_pointers(m_bufferGeometry.getRequiredPointerArraySize())
    {
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
    
    int size(int i) const override { ASSERT(i < N); return m_bufferGeometry.getDimensionExtents()[i]; }
    const std::array<int, N>& sizes() const override { return m_bufferGeometry.getDimensionExtents(); }
    
private:
    /** Build a const N-1 HyperBuffer view to this Hyperbuffer's data */
    const HyperBufferView<T, N-1> createSubBufferView(size_type index) const
    {
        ASSERT(index < this->size(0), "Index out of range");
        int offset = m_bufferGeometry.getDataArrayOffsetForHighestOrderSubDim(index);
        return HyperBufferView<T, N-1>(&m_externalData[offset], StdArrayOperations::shaveOffFirstElement(this->sizes()));
    }
    
    /** Build a N-1 HyperBuffer view to this Hyperbuffer's data */
    HyperBufferView<T, N-1> createSubBufferView(size_type index)
    {
        return std::as_const(*this).createSubBufferView(index);
    }
    
    const_pointer_type  getDataPointer_Nx() const override { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
           pointer_type getDataPointer_Nx()       override { return reinterpret_cast<pointer_type>(m_pointers.data()); }
               const T* getDataPointer_N1() const override { return *m_pointers.data(); }
                     T* getDataPointer_N1()       override { return *m_pointers.data(); }
    
private:
    friend IHyperBuffer<T, N, HyperBufferView<T, N>>;
    
    BufferGeometry<N> m_bufferGeometry;
    T* m_externalData;
    std::vector<T*> m_pointers;
};


// ====================================================================================================================
// MARK: - HyperBufferViewMD - manages existing multi-dimensional data (pure wrapper, owns neither pointers nor data)
template<typename T, int N>
class HyperBufferViewMD : public IHyperBuffer<T, N, HyperBufferViewMD<T, N>>
{
    using typename IHyperBuffer<T, N, HyperBufferViewMD<T, N>>::size_type;
    using typename IHyperBuffer<T, N, HyperBufferViewMD<T, N>>::pointer_type;
    using typename IHyperBuffer<T, N, HyperBufferViewMD<T, N>>::const_pointer_type;

public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    HyperBufferViewMD(pointer_type preAllocatedData, I... i) :
        m_dimensionExtents{static_cast<int>(i)...},
        m_externalData(preAllocatedData) {}
            
    /** Constructor that takes the extents of the dimensions as a std::array */
    HyperBufferViewMD(pointer_type preAllocatedData, std::array<int, N> dimensionExtents) :
        m_dimensionExtents(dimensionExtents),
        m_externalData(preAllocatedData) {}
    
    /** Constructor that takes the extents of the dimensions as a std::vector */
    HyperBufferViewMD(pointer_type preAllocatedData, std::vector<int> dimensionExtents) :
        m_externalData(preAllocatedData)
    {
        ASSERT(dimensionExtents.size() == N, "Incorrect number of dimension extents");
        std::copy(dimensionExtents.begin(), dimensionExtents.end(), m_dimensionExtents.begin());
    }
    
    int size(int i) const override { ASSERT(i < N); return m_dimensionExtents[i]; }
    const std::array<int, N>& sizes() const override { return m_dimensionExtents; }
    
private:
    /** Build a const N-1 HyperBuffer view to this Hyperbuffer's data */
    const HyperBufferViewMD<T, N-1> createSubBufferView(size_type index) const
    {
        ASSERT(index < this->size(0), "Index out of range");
        return HyperBufferViewMD<T, N-1>(m_externalData[index], StdArrayOperations::shaveOffFirstElement(this->sizes()));
    }
    
    /** Build a N-1 HyperBuffer view to this Hyperbuffer's data */
    HyperBufferViewMD<T, N-1> createSubBufferView(size_type index)
    {
        return std::as_const(*this).createSubBufferView(index);
    }

    const_pointer_type getDataPointer_Nx() const override { return m_externalData; }
         pointer_type  getDataPointer_Nx()       override { return m_externalData; }
              const T* getDataPointer_N1() const override { return reinterpret_cast<const T*>(m_externalData); }
                    T* getDataPointer_N1()       override { return reinterpret_cast<T*>(m_externalData); }
    
private:
    friend IHyperBuffer<T, N, HyperBufferViewMD<T, N>>;
    
    std::array<int, N> m_dimensionExtents;
    pointer_type m_externalData;
};

} // namespace slb
