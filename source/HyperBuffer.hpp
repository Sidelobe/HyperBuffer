//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <array>
#include <vector>

#include "TemplateUtils.hpp"
#include "IHyperBuffer.hpp"

#pragma once



// function to get offset/index on flat array based on var number of multi-dim indices
// recursive template?

template<int N>
struct BufferGeometry
{
    using dim_type = int;
    template<typename... I>
    explicit BufferGeometry(I... i) : m_dimensions{i...}
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }
    
    template<typename... I>
    int getOffsetForIndex(int d0, I... dn) const
    {
        int offset = getOffset(0, 0, d0, dn...);
        return offset;
    }
    
private:
    template<typename... I>
    int getOffset(int pos, int prevOffset, int d0) const
    {
        return prevOffset + d0;
    }
    // recursive function to calculate offset
    template<typename... I>
    int getOffset(int pos, int prevOffset, int d0, I... dn) const
    {
        int index = (d0 * m_dimensions[pos]) + prevOffset;
        return getOffset(pos + 1, index, dn...); // recusive call
    }
    
    std::array<int, N> m_dimensions;
};

// MARK: - HyperBufferOwning
template<typename T, int N>
class HyperBuffer : public IHyperBuffer<T, N>
{
    using size_type = typename IHyperBuffer<T, N>::size_type;
    using pointer_type = typename IHyperBuffer<T, N>::pointer_type;
    
public:
    template<typename... I>
    explicit HyperBuffer(I... i) :
        m_data(multiplyArgs(i...)),
        m_dataPointersSubdimensions(multiplyArgsExceptLast(i...)),
        m_dimensions{{i...}}
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
        // TODO: remove this uglyness
        auto begin = m_dimensions.begin();
        auto end = m_dimensions.end();
        std::vector<size_t> dimsSizeT(begin, end);
        m_dataPointers = callocArray<pointer_type>(dimsSizeT.data());
        
        //for (int m_dataPointersSubdimensions
    }
    
    ~HyperBuffer()
    {
         // TODO: remove this uglyness
        auto begin = m_dimensions.begin();
        auto end = m_dimensions.end();
        std::vector<size_t> dimsSizeT(begin, end);
        deleteArray(m_dataPointers, dimsSizeT.data());
    }
             
    decltype(auto) operator[] (size_type i) { return m_dataPointers[i]; }
                    
    pointer_type getDataPointers() const override
    {
        return m_dataPointers;
    }

private:
    /** we store the data in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    std::vector<T*> m_dataPointersSubdimensions; // to base class
    pointer_type m_dataPointers;
    std::array<int, N> m_dimensions;
};


// MARK: - HyperBufferPreAlloc
/** Construct from pre-allocated, multi-dimensional data */
template<typename T, int N>
class HyperBufferPreAlloc : public IHyperBuffer<T, N>
{
    using size_type = typename IHyperBuffer<T, N>::size_type;
    using pointer_type = typename IHyperBuffer<T, N>::pointer_type;
    
public:
    template<typename... I>
    explicit HyperBufferPreAlloc(pointer_type preAllocatedData, I... i) :
        m_dataPointers(preAllocatedData)
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }
    
    pointer_type getDataPointers() const override
    {
        return m_dataPointers;
    }
    
private:
    pointer_type m_dataPointers;
};

// MARK: - HyperBufferPreAllocFlat
/** Construct from pre-allocated, flat (1D) data */
template<typename T, int N>
class HyperBufferPreAllocFlat : public HyperBuffer<T, N>
{
    using size_type = typename IHyperBuffer<T, N>::size_type;
    using pointer_type = typename IHyperBuffer<T, N>::pointer_type;
    
public:
//    explicit HyperBufferPreAllocFlat(T* preAllocatedDataFlat, I... i)
//    {
//        // TODO:
//    }
    
};
