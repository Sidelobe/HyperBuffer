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
        m_bufferGeometry(i...),
        m_data(STL(m_bufferGeometry.getRequiredDataArraySize())),
        m_pointers(STL(HyperBufferBase<T, N>::getNumberOfPointers(i...)))
    {
        m_bufferGeometry.hookupPointerArrayToData(m_data.data(), m_pointers.data());
    }
    
    HyperBuffer(const HyperBuffer&) = default;
    HyperBuffer(HyperBuffer&&) = default;
    HyperBuffer<T, N>& operator= (const HyperBuffer&) = default;
    HyperBuffer<T, N>& operator= (HyperBuffer&&) = default;

    // https://stackoverflow.com/questions/5695548/public-friend-swap-member-function
    friend void swap(HyperBufferBase<T, N>& first, HyperBufferBase<T, N>& second) noexcept
    {
        using std::swap; // allow use of std::swap
        swap(first.m_bufferGeometry, second.m_bufferGeometry); // but select overloads, first
        swap(first.m_data, second.m_data);
        swap(first.m_pointers, second.m_pointers);
        // if swap(x, y) finds a better match, via ADL, it will use that instead; otherwise it falls back to std::swap
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
    
    /** All the data (innermost dimension) is stored in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    std::vector<T*> m_pointers;
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
