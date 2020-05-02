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
    using size_type = typename HyperBufferBase<T, N>::size_type;
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using subdim_pointer_type = typename HyperBufferBase<T, N>::subdim_pointer_type;
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
    
private:
    T& getTopDimensionData_N1(size_type i) override
    {
        ASSERT(N==1 && "this should only be called for N==1 !");
        return m_data[STL(i)];
    }
    
    pointer_type getDataPointer_Nx() override
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
    }
    
private:
    BufferGeometry<N> m_bufferGeometry;
    
    /** we store the data in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    std::vector<T*> m_pointers;
};


// MARK: - HyperBufferPreAllocFlat - manages existing 1D data
template<typename T, int N>
class HyperBufferPreAllocFlat : public HyperBufferBase<T, N>
{
    using size_type = typename HyperBufferBase<T, N>::size_type;
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using subdim_pointer_type = typename HyperBufferBase<T, N>::subdim_pointer_type;
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
    T& getTopDimensionData_N1(size_type i) override
    {
        ASSERT(N==1 && "this should only be called for N==1 !");
        return m_externalData[i];
    }
    
    pointer_type getDataPointer_Nx() override
    {
        return reinterpret_cast<pointer_type>(m_pointers.data());
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
    using size_type = typename HyperBufferBase<T, N>::size_type;
    using pointer_type = typename HyperBufferBase<T, N>::pointer_type;
    using subdim_pointer_type = typename HyperBufferBase<T, N>::subdim_pointer_type;
    using HyperBufferBase<T, N>::STL;
    
public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit HyperBufferPreAlloc(pointer_type preAllocatedData, I... i) :
        HyperBufferBase<T, N>(i...),
        m_externalData(preAllocatedData)
    {
        if (N == 1) {
            m_pointers.push_back(reinterpret_cast<T*>(preAllocatedData));
            
        } else {
            for (int j=0; j < this->m_dimensionExtents[0]; ++j) {
                m_pointers.push_back(reinterpret_cast<T*>(preAllocatedData[j]));
            }
        }
    }
    
private:
    T& getTopDimensionData_N1(size_type i) override
    {
        ASSERT(N==1 && "this should only be called for N==1 !");
        return m_pointers[0][i];
    }
    
    pointer_type getDataPointer_Nx() override
    {
        return m_externalData;
    }
    
private:
    pointer_type m_externalData;
    std::vector<T*> m_pointers;
};
