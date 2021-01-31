//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <memory>
#include <array>

#include "TemplateUtils.hpp"
#include "IntArrayOperations.hpp"

namespace slb
{

/**
 * This class performs the 'geometry' calculations for certain set of dimension extents.
 *
 * The underlying data model resembles two self-referencing flat arrays that contain all the pointers (for every
 * dimension except the lowest-order) and all the data (lowest-order dimension).
 */
template<int N>
class BufferGeometry
{
public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit BufferGeometry(I... i) : m_dimensionExtents{i...}
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }
    
    /** Constructor that takes the extents of the dimensions as a std::array */
    explicit BufferGeometry(const std::array<int, N>& dimensionExtents) : m_dimensionExtents(dimensionExtents) {}
    
    /** Constructor that takes the extents of the dimensions as a std::vector */
    explicit BufferGeometry(const std::vector<int>& dimensionExtents)
    {
        ASSERT(dimensionExtents.size() == N, "Incorrect number of dimension extents");
        std::copy(dimensionExtents.begin(), dimensionExtents.end(), m_dimensionExtents.begin());
    }

    const std::array<int, N>& getDimensionExtents() const { return m_dimensionExtents; }
    const int* getDimensionExtentsPointer() const { return m_dimensionExtents.data(); }
    
    /** @return the number of required data entries (lowest-order dimension) given the configured geometry */
    int getRequiredDataArraySize() const
    {
        return StdArrayOperations::product(m_dimensionExtents);
    }
    
    /** @return the number of required pointer entries given the configured geometry */
    int getRequiredPointerArraySize() const
    {
        return std::max(StdArrayOperations::sumOfCumulativeProductCapped(N-1, m_dimensionExtents), 1); // at least size 1
    }
    
    /**
     * Calculates the offset of where data that belongs to a specific (given index) highest-order sub-dimension starts.
     * e.g. if the highest-order dimension's extent is 2, all data for index=0 is located in the first half of
     * the **data** array and the all data for index=1 in the second half.
     */
    int getDataArrayOffsetForHighestOrderSubDim(int index) const
    {
        const int totalNumDataEntries = getRequiredDataArraySize();
        ASSERT(index < m_dimensionExtents[0], "Index out of range");
        ASSERT(totalNumDataEntries % m_dimensionExtents[0] == 0, "Internal error in buffer geometry!");
        return index * totalNumDataEntries / m_dimensionExtents[0];
    }
    
    /**
     * Set up the supplied pointer array as a self-referencing array and point the lowest dimension
     * pointers at the supplied data array.
     *
     * @param dataArray an array of T. Size must match result given by getRequiredDataArraySize()
     * @param pointerArray an array of T*. Size must match result given by getRequiredPointerArraySize()
     */
    template<typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
    void hookupPointerArrayToData(T* dataArray, T** pointerArray) const
    {
        if (N == 1) {
            pointerArray[0] = dataArray;
            return;
        }
        
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
    int hookupHigherDimPointers(T** pointerArray, int arrayIndex, int dimIndex) const
    {
        if (dimIndex >= N-2) {
            return arrayIndex; // end recursion
        }

        int startOfNextDimension = StdArrayOperations::sumOfCumulativeProductCapped(dimIndex+1, m_dimensionExtents);
        int numPointersInThisDimension = StdArrayOperations::productCapped(dimIndex+1, m_dimensionExtents);

        for (int index = 0; index < numPointersInThisDimension; ++index) {
            int nextDimExtent = m_dimensionExtents[static_cast<unsigned>(dimIndex) + 1];
            int offset = startOfNextDimension + nextDimExtent * index;
            // hook up pointer to element of next dimension
            pointerArray[arrayIndex + index] = reinterpret_cast<T*>(&(pointerArray)[offset]);
        }
        
        // recursive call to lower-order dimension
        arrayIndex += numPointersInThisDimension;
        return hookupHigherDimPointers(pointerArray, arrayIndex, dimIndex+1);
    }
                                                     
private:
    std::array<int, N> m_dimensionExtents;
    
};

} // namespace slb
