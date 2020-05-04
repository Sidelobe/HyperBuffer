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
    using HyperBufferBase<T, N>::STL;

public:
    template<typename... I>
    explicit HyperBuffer(I... i) :
        HyperBufferBase<T, N>(i...),
        m_bufferGeometry(i...)
    {
        m_data = new T[m_bufferGeometry.getRequiredDataArraySize()];
        m_pointers = new T*[m_bufferGeometry.getRequiredPointerArraySize()];
        m_bufferGeometry.hookupPointerArrayToData(m_data, m_pointers);
    }
    
    ~HyperBuffer()
    {
        if (m_pointers != nullptr) {
            delete[] m_pointers;
        }
        if (m_data != nullptr) {
            delete[] m_data;
        }
    }
    
    HyperBuffer(const HyperBuffer& other) : HyperBuffer(other.m_bufferGeometry.getDimensionExtents())
    {
        std::memcpy(m_data, other.m_data, m_bufferGeometry.getRequiredDataArraySize() * sizeof(T));
        std::memcpy(m_pointers, other.m_pointers, m_bufferGeometry.getRequiredPointerArraySize() * sizeof(T*));
    }
    
    HyperBuffer(HyperBuffer&& other) noexcept :
        HyperBufferBase<T, N>(other.m_bufferGeometry.getDimensionExtents()),
        m_bufferGeometry(other.m_bufferGeometry)
    {
        swap(*this, other);
    }
    
    HyperBuffer<T, N>& operator= (const HyperBuffer& rhs)
    {
        if (this != &rhs) {
            // copy/swap idiom: (de-)allocates memory
            HyperBuffer<T, N> tmp(rhs);
            swap(*this, tmp);
        }
        return *this;
    }
    
    HyperBuffer<T, N> const & operator= (HyperBuffer&& rhs) noexcept
    {
        if (this != &rhs) {
            swap(*this, rhs); // non-copying
        }
        return *this;
    }

    friend void swap(HyperBuffer<T, N>& first, HyperBuffer<T, N>& second) noexcept
    {
        using std::swap;
        swap(static_cast<HyperBufferBase<T, N>&>(first), static_cast<HyperBufferBase<T, N>&>(second));
        swap(first.m_bufferGeometry, second.m_bufferGeometry);
        swap(first.m_data, second.m_data);
        swap(first.m_pointers, second.m_pointers);
    }
    
private:
    pointer_type getDataPointer_Nx() const override
    {
        return reinterpret_cast<pointer_type>(m_pointers);
    }
    
    T* getDataPointer_N1() const override
    {
        return *m_pointers;
    }
    
private:
    BufferGeometry<N> m_bufferGeometry;
    
    /** All the data (innermost dimension) is stored in a 1D structure and access with offsets to simulate multi-dimensionality */
    T* m_data = nullptr;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    T** m_pointers = nullptr;
};


// MARK: - HyperBufferPreAllocFlat - manages existing 1D data
template<typename T, int N>
class HyperBufferPreAllocFlat : public HyperBufferBase<T, N>
{
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using HyperBufferBase<T, N>::STL;
    
public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBufferPreAllocFlat(T* preAllocatedDataFlat, I... i) :
        HyperBufferBase<T, N>(i...),
        m_bufferGeometry(i...),
        m_externalData(preAllocatedDataFlat),
        m_pointers(STL(HyperBufferBase<T, N>::getNumberOfPointers(i...)))
    {
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
   
private:
    pointer_type getDataPointer_Nx() const override
    {
        return reinterpret_cast<pointer_type>(const_cast<T**>(m_pointers.data()));
    }
    
    T* getDataPointer_N1() const override
    {
        return *(m_pointers.data());
    }
    
private:
    BufferGeometry<N> m_bufferGeometry;
    T* m_externalData;
    std::vector<T*> m_pointers;
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
