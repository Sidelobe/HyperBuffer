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

namespace OffsetCalculation
{
/** Calculates the product of the elements in range [start, end[ of the supplied array */
template<int N>
static int arrayProduct(int const* array, int start = 0, int end = N)
{
    int product = 1;
    for (int i = start; i < end; ++i) {
        product *= array[i];
    }
    return product;
}

template<int Nmax>
int getOffset(int dimIndex, int cumulativeOffset, int const* dimExtents, int d0)
{
    return cumulativeOffset + d0; // lowest-order dimension (dimIndex = N)
}

template<int Nmax, typename... I>
int getOffset(int dimIndex, int cumulativeOffset, int const* dimExtents, int dn, I... dk)
{
    int p = arrayProduct<Nmax>(dimExtents, dimIndex, Nmax); // Size of a block of data "above" this dimIndex
    int index = p * dn + cumulativeOffset;
    return getOffset<Nmax>(dimIndex+1, index, dimExtents, dk...); // recusive call
}

} // namespace OffsetCalculation

template<int N>
class BufferGeometry
{
public:
    /** Take the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit BufferGeometry(I... i) : m_dimensions{i...}
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }

    /** The data is saved in row-major ordering */
    template<typename... I>
    int getOffsetInDataArray(int dn, I... dk) const
    {
        static_assert(sizeof...(I) == N-1, "Incorrect number of arguments");
        return OffsetCalculation::getOffset<N>(1, 0, m_dimensions, dn, dk...);  // NOTE: start with dimIndex=1 - don't care about highest dimension's value
    }
    
    /** DimIdx corresponds to index in dimensions array, i.e. DimIdx=0 is the highest-order dimension */
    template<int DimIdx, typename... I>
    int getOffsetInPointerArray(int dn, I... dk) const
    {
        static_assert(sizeof...(I) == DimIdx, "Number of arguments should be DimIdx+1");
        static_assert(DimIdx < N-1, "Can only get pointers for any but the lowest-order dimension");

        int sumOfAllHigherDimPointers = 0;
        // --> sum of cumulative product "stopped early"
        for (int i=0; i < DimIdx; ++i) {
            int p = OffsetCalculation::arrayProduct<N>(m_dimensions, 0, i+1);
            sumOfAllHigherDimPointers += p;
        }
        
        // Calcualte offset within DimIdx
        std::cout << "Getting: <DimIndex=" << DimIdx <<"> " << dn << " ";
        int values[]{ dk... };
        if (sizeof...(dk) > 0){
            for (int i=0; i< sizeof...(dk); ++i) {
                std::cout << values[i] << " ";
            }
        }
        std::cout << "sumOfAllHigherDimPointers=" << sumOfAllHigherDimPointers;
        std::cout << " ... getOffsetMinusOne(DimIdx)=" << OffsetCalculation::getOffset<N-1>(1, 0, m_dimensions, dn, dk...) << std::endl;
        return sumOfAllHigherDimPointers + OffsetCalculation::getOffset<N-1>(1, 0, m_dimensions, dn, dk...);
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
    
private:
    int m_dimensions[N];
    
};
