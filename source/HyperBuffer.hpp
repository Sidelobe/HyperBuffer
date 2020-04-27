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

            
// MARK: - HyperBufferOwning
template<typename T, int N>
class HyperBuffer : public HyperBufferBase<T, N>
{
    using size_type = typename HyperBufferBase<T, N>::size_type;
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
        this->assignPointers(m_pointers.data());
    }
    
    // TODO: std::array ctor
private:
    T& getTopDimensionData_N1(size_type i) override
    {
        assert(N==1 && "this should only be called for N==1 !");
        return m_data[STL(i)];
    }
    
    subdim_pointer_type getTopDimensionData_Nx(size_type i) override
    {
        assert(N>1 && "this should only be called for N>1 !");
        assert((i < HyperBufferBase<T, N>::m_dimensionExtents[0]) && "index out range");
        // TODO: should we return a reference here --??
        // TODO: return sub-buffer? something like: HyperBuffer<T, N-1>(*this, i)
        
        // syntax weirdness: https://stackoverflow.com/questions/4942703/
        int offset = m_bufferGeometry.template getOffsetInPointerArray<0>(i);
        return reinterpret_cast<subdim_pointer_type>(m_pointers[STL(offset)]);
    }

private:
    BufferGeometry<N> m_bufferGeometry;
    
    /** we store the data in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    std::vector<T*> m_pointers;
};


// MARK: - HyperBufferPreAllocFlat
/** Construct from pre-allocated, flat (1D) data */
template<typename T, int N>
class HyperBufferPreAllocFlat : public HyperBufferBase<T, N>
{
    using size_type = typename HyperBufferBase<T, N>::size_type;
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
        this->assignPointers(m_pointers.data());
    }
   
private:
    T& getTopDimensionData_N1(size_type i) override
    {
        assert(N==1 && "this should only be called for N==1 !");
        return m_externalData[i];
    }
    
    subdim_pointer_type getTopDimensionData_Nx(size_type i) override
    {
        assert(N>1 && "this should only be called for N>1 !");
        assert((i < HyperBufferBase<T, N>::m_dimensionExtents[0]) && "index out range");
        // syntax weirdness: https://stackoverflow.com/questions/4942703/
        int offset = m_bufferGeometry.template getOffsetInPointerArray<0>(i);
        return reinterpret_cast<subdim_pointer_type>(m_pointers[STL(offset)]);
    }

    
private:
    BufferGeometry<N> m_bufferGeometry;
    T* m_externalData;
    std::vector<T*> m_pointers;
};



// MARK: - HyperBufferPreAlloc
/** Construct from pre-allocated, multi-dimensional data */
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
        this->assignPointers(preAllocatedData);
    }
    
private:
    T& getTopDimensionData_N1(size_type i) override
    {
        assert(N==1 && "this should only be called for N==1 !");
        return m_externalData[i];
    }
    
    subdim_pointer_type getTopDimensionData_Nx(size_type i) override
    {
        assert(N>1 && "this should only be called for N>1 !");
        assert((i < HyperBufferBase<T, N>::m_dimensionExtents[0]) && "index out range");
        return reinterpret_cast<subdim_pointer_type>(m_externalData[i]);
    }

    
private:
    pointer_type m_externalData;
};
