//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include "TestCommon.hpp"

#include "HyperBuffer.hpp"
#include "MemorySentinel.hpp"

void verifyBuffer(const HyperBufferBase<int, 3>& b)
{
    REQUIRE (b.dim(0) == 3);
    REQUIRE (b.dim(1) == 2);
    REQUIRE (b.dim(2) == 8);
    REQUIRE (b[1][0][5] == 333);
    REQUIRE (b[2][1][3] == -666);
}

TEST_CASE("Copy/Move a HyperBuffer with internal allocation")
{
    constexpr int N = 3;
    std::array<int, 3> dims {3, 2, 8};
    HyperBuffer<int, N> buffer(dims);
    buffer[1][0][5] = 333;
    buffer[2][1][3] = -666;

    // TODO: test N=1
    // Copy assignment operator
    HyperBuffer<int, N> bufferCopy = buffer;
    verifyBuffer(bufferCopy);
    verifyBuffer(buffer); // original remains untouched
    
    // Copy Ctor
    HyperBuffer<int, N> bufferCopyCtor(buffer);
    verifyBuffer(bufferCopy);
    verifyBuffer(buffer); // original remains untouched
    
    // verify no memory is allocated during copy to buffer with same size
    HyperBuffer<int, N> bufferCopySameSize(dims);
    {
        ScopedMemorySentinel sentinel;
        bufferCopySameSize = buffer;
    }
    
    // this will work, but will allocate memoery
    std::array<int, 3> dimsSmaller {2, 2, 6};
    HyperBuffer<int, N> bufferCopySmallerSize(dimsSmaller);
    bufferCopySmallerSize = buffer;
    verifyBuffer(bufferCopy);
    verifyBuffer(buffer); // original remains untouched
    
    // Move assignment operator
    {
        HyperBuffer<int, N> bufferMovedFrom = buffer; // working copy
        HyperBuffer<int, N> bufferMovedTo = std::move(bufferMovedFrom);
        verifyBuffer(bufferMovedTo);
        REQUIRE(bufferMovedFrom.data() == nullptr);
    }
    
    // verify no memory is allocated during move assignment
    {
        HyperBuffer<int, N> bufferMovedFrom = buffer; // working copy
        HyperBuffer<int, N> bufferMovedToPreAlloc(dims);
        {
            //ScopedMemorySentinel sentinel;
            bufferMovedToPreAlloc = std::move(bufferMovedFrom);
        }
        verifyBuffer(bufferMovedToPreAlloc);
        REQUIRE(bufferMovedFrom.data() == nullptr);
    }
    
    // Move Ctor
    {
        HyperBuffer<int, N> bufferMovedFrom = buffer;
        HyperBuffer<int, N> bufferMovedToCtor(std::move(bufferMovedFrom));
        verifyBuffer(bufferMovedToCtor);
        REQUIRE(bufferMovedFrom.data() == nullptr);
    }
    
    // verify no memory is allocated during move construction
    {
        HyperBuffer<int, N> bufferMovedFrom = buffer; // working copy
        {
            //ScopedMemorySentinel sentinel;
            HyperBuffer<int, N> bufferMovedCtor(std::move(bufferMovedFrom));
        }
        REQUIRE(bufferMovedFrom.data() == nullptr);
    }
}

TEST_CASE("Copy/Move a HyperBuffer with external, flat allocation")
{
    constexpr int N = 3;
    int preAllocData[3*2*8] {0};
    std::array<int, 3> dims {3, 2, 8};
    HyperBufferPreAllocFlat<int, N> buffer(preAllocData, dims);
    buffer[1][0][5] = 333;
    buffer[2][1][3] = -666;

    // Copy assignment operator
    HyperBufferPreAllocFlat<int, N> bufferCopy = buffer;
    verifyBuffer(bufferCopy);
    verifyBuffer(buffer); // original remains untouched
    REQUIRE(bufferCopy[0] == buffer[0]); // copy points to the same data
    REQUIRE(bufferCopy[0][1] == buffer[0][1]);
    REQUIRE(bufferCopy[2][0] == buffer[2][0]);
    
    // Copy Ctor - verify no memory is allocated and memory
    int preAllocDataSameSize[getRawArrayLength(preAllocData)] {0};
    HyperBufferPreAllocFlat<int, N> bufferCopyCtor(preAllocDataSameSize, 3, 2, 8);
    {
        ScopedMemorySentinel sentinel;
        bufferCopyCtor = buffer;
    }
    verifyBuffer(bufferCopyCtor);
    verifyBuffer(buffer); // original remains untouched
    REQUIRE(bufferCopy[0] == buffer[0]); // copy points to the same data
    REQUIRE(bufferCopy[0][1] == buffer[0][1]);
    REQUIRE(bufferCopy[2][0] == buffer[2][0]);

    // Move assignment operator
    {
        HyperBufferPreAllocFlat<int, N> bufferMovedFrom = buffer; // working copy
        HyperBufferPreAllocFlat<int, N> bufferMovedTo = std::move(bufferMovedFrom);
        verifyBuffer(bufferMovedTo);
        REQUIRE(bufferMovedFrom.data() == nullptr);
    }
    
    // verify no memory is allocated during move assignment
    {
        HyperBufferPreAllocFlat<int, N> bufferMovedFrom = buffer; // working copy
        int preAllocData2[getRawArrayLength(preAllocData)] {0};
        HyperBufferPreAllocFlat<int, N> bufferMovedToPreAlloc(preAllocData2, dims);
        {
            //ScopedMemorySentinel sentinel;
            bufferMovedToPreAlloc = std::move(bufferMovedFrom);
        }
        verifyBuffer(bufferMovedToPreAlloc);
        REQUIRE(bufferMovedFrom.data() == nullptr);
    }

    // Move Ctor
    {
        HyperBufferPreAllocFlat<int, N> bufferMovedFrom = buffer;
        HyperBufferPreAllocFlat<int, N> bufferMovedToCtor(std::move(bufferMovedFrom));
        verifyBuffer(bufferMovedToCtor);
        REQUIRE(bufferMovedFrom.data() == nullptr);
    }

    // verify no memory is allocated during move construction
    {
        HyperBufferPreAllocFlat<int, N> bufferMovedFrom = buffer; // working copy
        {
            //ScopedMemorySentinel sentinel;
            HyperBufferPreAllocFlat<int, N> bufferMovedCtor(std::move(bufferMovedFrom));
        }
        REQUIRE(bufferMovedFrom.data() == nullptr);
    }
}

