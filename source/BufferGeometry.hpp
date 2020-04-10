//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <memory>
#include <array>

#include "TemplateUtils.hpp"

template<int N>
struct BufferGeometry
{
    using dim_type = int;
    template<typename... I>
    explicit BufferGeometry(I... i) : m_dimensions{i...}
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }
    
    /** The data is saved in row-major ordering */
    template<typename... I>
    int getOffsetForIndex(int dn, I... dk) const
    {
        static_assert(sizeof...(I) == N-1, "Incorrect number of arguments");
//        std::cout << "Getting: " << dn << " ";
//        int values[]{ dk... };
//        if (sizeof...(dk) > 0){
//            for (int i=0; i< sizeof...(dk); ++i) {
//                std::cout << values[i] << " ";
//            }
//            std::cout << std::endl;
//        }
        // NOTE: start with pos=1 - we don't care about highest dimension's value here
        return getOffset(1, 0, dn, dk...);
    }
    
    
    
    template<int DimIdx, typename... I>
    int getOffsetInPointerArray(int dn, I... dk) const
    {
        static_assert(sizeof...(I) == DimIdx, "Number of arguments should be DimIdx+1");
 
        if (DimIdx == 0) {
            return dn;
        }
         return m_dimensions[0] + getOffset(DimIdx, 0, dn, dk...);
    }
    
    int getOffsetForDimensionStart(int dimensionIndex) const
    {
        // TODO: generic way std::make_index_sequence<N>{} ?
        if (dimensionIndex == 0) {
            return getOffset(0, 0, 0);
        } else if (dimensionIndex == 1) {
            return getOffset(0, 0, 0, 0);
        } else if (dimensionIndex == 2) {
            return getOffset(0, 0, 0, 0, 0);
        } else if (dimensionIndex == 3) {
            return getOffset(0, 0, 0, 0, 0);
        } else {
            assert(true && "unhandled case");
            return 0;
        }
    }
    
    // NOTE: we can choose any pointer type here, since all pointers types are same size
    template<typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
    void hookupPointerArrayToData(const T* dataArray, T** pointerArray)
    {
        // Get
        
        // skip lowest dimension
//        for (int dimIndex = m_dimensions.size()-1; dimIndex > 0; --dimIndex) {
//            for (int dimValueIndex = 0; dimValueIndex < m_dimensions[dimIndex]; ++dimValueIndex) {
//                int dimStart = getOffsetForDimensionStart(dimValueIndex);
////                std::cout << "dimStart: " << dimStart;
//                int offset = dimStart + dimValueIndex;
////                std::cout << " offset: " << offset << std::endl;
//                pointerArray[offset] = (float*)(dataArray + offset);
//            }
//        }
    }
    
    std::array<int, N> getDimensions() const { return m_dimensions; }
    
private:
    template<typename... I>
    int getOffset(int pos, int prevOffset, int d0) const
    {
        return prevOffset + d0;
    }
    
    template<typename... I>
    int getOffset(int pos, int prevOffset, int dn, I... dk) const
    {
        int index = m_dimensions[pos] * dn + prevOffset;
        return getOffset(pos + 1, index, dk...); // recusive call
    }
    
    std::array<int, N> m_dimensions;
};
