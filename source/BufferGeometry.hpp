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
        return getOffset<N>(1, 0, dn, dk...);  // NOTE: start with dimIndex=1 - don't care about highest dimension's value
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
            int p = StdArrayOperations::productOverRange(0, i+1, m_dimensionExtents);
            sumOfAllHigherDimPointers += p;
        }
        
        // Calcualte offset within DimIdx
//        std::cout << "Getting: <DimIndex=" << DimIdx <<"> " << dn << " ";
//        int values[]{ dk... };
//        if (sizeof...(dk) > 0){
//            for (int i=0; i< sizeof...(dk); ++i) {
//                std::cout << values[i] << " ";
//            }
//        }
//        std::cout << "sumOfAllHigherDimPointers=" << sumOfAllHigherDimPointers;
//        std::cout << " ... getOffsetMinusOne(DimIdx)=" << OffsetCalculation::getOffset<N-1>(1, 0, m_dimensionExtents, dn, dk...) << std::endl;
        int offset = getOffset<N-1>(1, 0, dn, dk...);
        return sumOfAllHigherDimPointers + offset;
    }
    
public:
    // NOTE: we can choose any pointer type here, since all pointers types are same size
    template<typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
    void hookupPointerArrayToData(T* dataArray, T** pointerArray)
    {
        static_assert(N > 1, "Cannt use this function in 1-dimensional case");
       
        
        // for every dimension, we hook it up to the next underlying/child dimension
        // we need:  this dimension's extent Dn
        
        // first child is at offset +Dn  from the start of this dimension
        // second child is at offset +Dn+1 ...
        
        // we call the child dimension (recursion) -- pass offset +D(n) + D(n+1)
        
        std::cout << "Hooking up pointers for dimensions ";
        for (int dimIndex = 0; dimIndex < N; ++dimIndex) {
            std::cout << m_dimensionExtents[dimIndex] << " ";
        }
        std::cout << std::endl;
        
        
        // MARK: - Intertwine pointer array: connect all pointers to itself -- skips 2 lowest-order dimensions
        hookupHigherDimPointers(pointerArray, 0, 0);
        
        // MARK: - Calculate offset of first pointer that points to data rather then another pointer (start of 2nd-lowest dim)
        std::array<int, N> dimsArray;
        for (int i=0; i < N; ++i) {
             dimsArray[i] = m_dimensionExtents[i];
        }
        int nMinus2DimStart = VarArgOperations::apply([](auto&&... args)
        {
            if (N < 3) { return 0; }
            return VarArgOperations::sumOfCumulativeProductCapped(N-2, std::forward<decltype(args)>(args)...);
        }, dimsArray);
        
        // MARK: - Get number of data pointers (length of 2nd-lowest dim)
        int numDataPointers = StdArrayOperations::productCapped(N-1, m_dimensionExtents);
//        int numDataPointers = VarArgOperations::apply([](auto&&... args)
//        {
//            if (N < 2) { return 0; }
//            return VarArgOperations::productCapped(N-1, std::forward<decltype(args)>(args)...);
//        }, dimsArray);
        
        // MARK: - Hook up pointer that point to data (second lowest-order dimension)
        for (int i=0; i < numDataPointers; ++i) {
            int dataOffset = i * m_dimensionExtents[N-1];
            pointerArray[nMinus2DimStart + i] = &dataArray[dataOffset];
        }
    }
    
    template<typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
    void hookupHigherDimPointers(T** pointerArray, int arrayIndex, int dimIndex)
    {
        if (dimIndex >= N-2) {
            return;
        }

        // SumCumProd Capped
        auto intTuple = VarArgOperations::makeIntTuple(m_dimensionExtents);
        int sumCumProdUntilDimIndex = VarArgOperations::apply([dimIndex](auto&&... args)
        {
            return VarArgOperations::sumOfCumulativeProductCapped(dimIndex+1, std::forward<decltype(args)>(args)...);
        }, intTuple);

        int prodUntilDimIndex = StdArrayOperations::productCapped(dimIndex+1, m_dimensionExtents);

        
        for (int index = 0; index < prodUntilDimIndex; ++index) {
            std::cout << "DimIndex=" << dimIndex << " Element= " << index <<  std::endl;
            int offset = sumCumProdUntilDimIndex + m_dimensionExtents[dimIndex+1] * index;
            std::cout << "[" << arrayIndex+index << "] = offset " << offset << std::endl;
            pointerArray[arrayIndex + index] = (T*) &(pointerArray)[offset];
        }
        
        // recursive call to lower-order dimension
        hookupHigherDimPointers(pointerArray, arrayIndex+prodUntilDimIndex, dimIndex+1);
        
    }
    
private:
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
