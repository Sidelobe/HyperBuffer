//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <memory>
#include <array>
#include <iostream>

#include "TemplateUtils.hpp"
#include "IntArrayOperations.hpp"

template<int N>
class BufferGeometry
{
public:
    /** Take the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit BufferGeometry(I... i) : m_dimensionExtents{i...}
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }

    /** The data is saved in row-major ordering */
    template<typename... I>
    int getOffsetInDataArray(int dn, I... dk) const
    {
        static_assert(sizeof...(I) == N-1, "Incorrect number of arguments");
        return getOffset<N>(1, 0, dn, dk...);  // we start with dimIndex=1 - don't care about highest dimension's value
    }
    
    /** DimIdx corresponds to index in dimensions array, i.e. DimIdx=0 is the highest-order dimension */
    template<int DimIdx, typename... I>
    int getOffsetInPointerArray(int dn, I... dk) const
    {
        static_assert(sizeof...(I) == DimIdx, "Number of arguments should be DimIdx+1");
        static_assert(DimIdx < N-1, "Can only get pointers for any but the lowest-order dimension");

        int startOfThisDimension = StdArrayOperations::sumOfCumulativeProductCapped(DimIdx, m_dimensionExtents);
        return startOfThisDimension + getOffset<N-1>(1, 0, dn, dk...); // We ignore the 'data dimension', therefore N-1
    }
    
public:
    // NOTE: we can choose any pointer type here, since all pointers types are same size
    template<typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
    void hookupPointerArrayToData(T* dataArray, T** pointerArray)
    {
        static_assert(N > 1, "Cannt use this function in 1-dimensional case");
        
        // Intertwine pointer array: connect all pointers to itself -- skips 2 lowest-order dimensions
        int dataPointerStartOffset = hookupHigherDimPointers(pointerArray, 0, 0);
  
        // Get number of data pointers (length of 2nd-lowest dim)
        int numDataPointers = StdArrayOperations::productCapped(N-1, m_dimensionExtents);

        //Hook up pointer that point to data (second lowest-order dimension)
        for (int i=0; i < numDataPointers; ++i) {
            int offsetInDataArray = i * m_dimensionExtents[N-1];
            pointerArray[dataPointerStartOffset + i] = &dataArray[offsetInDataArray];
        }
    }
    
private:
    template<typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
    int hookupHigherDimPointers(T** pointerArray, int arrayIndex, int dimIndex)
    {
        if (dimIndex >= N-2) {
            return arrayIndex; // end recursion
        }

        int startOfNextDimension = StdArrayOperations::sumOfCumulativeProductCapped(dimIndex+1, m_dimensionExtents);
        int numPointersInThisDimension = StdArrayOperations::productCapped(dimIndex+1, m_dimensionExtents);

        for (int index = 0; index < numPointersInThisDimension; ++index) {
            int offset = startOfNextDimension + m_dimensionExtents[dimIndex+1] * index;
            pointerArray[arrayIndex + index] = (T*) &(pointerArray)[offset]; // hook up pointer to element of next dimension
        }
        
        // recursive call to lower-order dimension
        arrayIndex += numPointersInThisDimension;
        return hookupHigherDimPointers(pointerArray, arrayIndex, dimIndex+1);
    }

    template<std::size_t Nmax>
    int getOffset(int dimIndex, int cumulativeOffset, int d0) const
    {
        return cumulativeOffset + d0; // lowest-order dimension (dimIndex = N)
    }

    template<std::size_t Nmax, typename... I>
    int getOffset(int dimIndex, int cumulativeOffset, int dn, I... dk) const
    {
        int p = StdArrayOperations::productOverRange(dimIndex, Nmax, m_dimensionExtents); // Size of a block of data "above" this dimIndex
        int index = p * dn + cumulativeOffset;
        return getOffset<Nmax>(dimIndex+1, index, dk...); // recusive call
    }
                                                     
private:
    std::array<int, N> m_dimensionExtents;
    
};
