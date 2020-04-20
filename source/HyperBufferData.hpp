//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <array>
#include <vector>

#include "TemplateUtils.hpp"

#pragma once


// function to get offset/index on flat array based on var number of multi-dim indices
// recursive template?

template<typename T>
struct DimensionData
{
    using dimension_index = int;
    using size_type = size_t;
    
    DimensionData(int size) : m_size(size), m_subDimensionPointers(size) {}
    int m_size;
    std::vector<T*> m_subDimensionPointers;
//
//    size_type getIndex(dimension_index pos, size_type prevIndex, size_type m1, T...k1) const
//    {
//        size_type index = (prevIndex* m_dimensions[pos]) + m1;
//
//        if constexpr (sizeof...(k1) > 0)
//        {
//            return GetIndex(pos + 1, index, k1...);
//        }
//        else
//        {
//            return index;
//        }
//    }

    
};

template<typename T, int N>
class HyperBufferData
{
    using size_type = int;
    using pointer_type = typename add_pointers_to_type<T, N>::type;
    
public:
    virtual ~HyperBufferData() {}
    virtual pointer_type getDataPointers() const = 0;
    
public:
    std::array<int, N> m_dimensions;
    //std::array<DimensionData<T>, N> m_dimensionData;
    
protected:
    template<typename... I>
    HyperBufferData(I... i) : m_dimensions({ {static_cast<int>(i)...} })
    {
        
    }
    

};

// MARK: - HyperBufferOwning
template<typename T, int N>

class HyperBufferDataOwning : public HyperBufferData<T, N>
{
    using size_type = int;
    using pointer_type = typename add_pointers_to_type<T, N>::type;
    
public:
    template<typename... I>
    explicit HyperBufferDataOwning(I... i) :
        HyperBufferData<T, N>(i...),
        m_data(multiplyArgs(i...))
    {
        // TODO: remove this uglyness
        auto begin = HyperBufferData<T, N>::m_dimensions.begin();
        auto end = HyperBufferData<T, N>::m_dimensions.end();
        std::vector<size_t> dimsSizeT(begin, end);
        m_dataPointers = callocArray<pointer_type>(dimsSizeT.data());
    }
    
    ~HyperBufferDataOwning()
    {
         // TODO: remove this uglyness
        auto begin = HyperBufferData<T, N>::m_dimensions.begin();
        auto end = HyperBufferData<T, N>::m_dimensions.end();
        std::vector<size_t> dimsSizeT(begin, end);
        deleteArray(m_dataPointers, dimsSizeT.data());
    }
    
    pointer_type getDataPointers() const override
    {
        return m_dataPointers;
    }

private:
    /** we store the data in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    pointer_type m_dataPointers;
};

// MARK: - HyperBufferPreAlloc
template<typename T, int N>
class HyperBufferDataPreAlloc : public HyperBufferData<T, N>
{
    using pointer_type = typename add_pointers_to_type<T, N>::type;
    
public:
    template<typename... I>
    explicit HyperBufferDataPreAlloc(pointer_type preAllocatedData, I... i) :
        HyperBufferData<T, N>(i...),
        m_dataPointers(preAllocatedData)
    {
        
    }
    
    pointer_type getDataPointers() const override
    {
        return m_dataPointers;
    }
    
private:
    pointer_type m_dataPointers;
};
