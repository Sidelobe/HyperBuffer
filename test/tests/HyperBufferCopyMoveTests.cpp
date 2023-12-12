//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2023 Lorenz Bucher - all rights reserved
//  https://github.com/Sidelobe/HyperBuffer

#include "TestCommon.hpp"

#include "HyperBuffer.hpp"
#include "MemorySentinel.hpp"

#if defined(_MSC_VER) && defined(_DEBUG) &&_ITERATOR_DEBUG_LEVEL > 1
#define MSVC_DEBUG 1
#else
#define MSVC_DEBUG 0
#endif

using namespace slb;

template<typename U>
void verifyBuffer(const U& b)
{
    REQUIRE (b.size(0) == 3);
    REQUIRE (b.size(1) == 2);
    REQUIRE (b.size(2) == 8);
    REQUIRE (b[1][0][5] == 333);
    REQUIRE (b[2][1][3] == -666);
}

static_assert(std::is_copy_constructible<HyperBuffer<int, 3>>::value, "should be Copy-Constructible");
static_assert(std::is_copy_assignable<HyperBuffer<int, 3>>::value, "should be Copy-Assignable");
static_assert(std::is_nothrow_move_constructible<HyperBuffer<int, 3>>::value, "should be noexcept Move-Constructible");
static_assert(std::is_nothrow_move_assignable<HyperBuffer<int, 3>>::value, "should be noexcept Move-Assignable");

static_assert(std::is_copy_constructible<HyperBufferView<int, 3>>::value, "should be Copy-Constructible");
static_assert(std::is_copy_assignable<HyperBufferView<int, 3>>::value, "should be Copy-Assignable");
static_assert(std::is_nothrow_move_constructible<HyperBufferView<int, 3>>::value, "should be noexcept Move-Constructible");
static_assert(std::is_nothrow_move_assignable<HyperBufferView<int, 3>>::value, "should be noexcept Move-Assignable");

static_assert(std::is_copy_constructible<HyperBufferViewNC<int, 3>>::value, "should be Copy-Constructible");
static_assert(std::is_copy_assignable<HyperBufferViewNC<int, 3>>::value, "should be Copy-Assignable");
static_assert(std::is_nothrow_move_constructible<HyperBufferViewNC<int, 3>>::value, "should be noexcept Move-Constructible");
static_assert(std::is_nothrow_move_assignable<HyperBufferViewNC<int, 3>>::value, "should be noexcept Move-Assignable");

TEST_CASE("Copy/Move a HyperBuffer with internal allocation")
{
    constexpr int N = 3;
    std::array<int, N> dims {3, 2, 8};
    HyperBuffer<int, N> buffer(dims);
    buffer[1][0][5] = 333;
    buffer[2][1][3] = -666;

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
    
    // this will work, but will allocate memory
    std::array<int, 3> dimsSmaller {2, 2, 6};
    HyperBuffer<int, N> bufferCopySmallerSize(dimsSmaller);
    bufferCopySmallerSize = buffer;
    verifyBuffer(bufferCopy);
    verifyBuffer(buffer); // original remains untouched
    
    // Move assignment operator
    {
        HyperBuffer<int, N> bufferMovedFrom = buffer; // working copy
        HyperBuffer<int, N> bufferMovedTo(1, 1, 1);
        bufferMovedTo = std::move(bufferMovedFrom);
        verifyBuffer(bufferMovedTo);
        REQUIRE(bufferMovedTo.data() != nullptr);
    }
    
#if MSVC_DEBUG == 0
    // verify no memory is allocated during move assignment
    {
        HyperBuffer<int, N> bufferMovedFrom = buffer; // working copy
        
        // Can't use Scoped Sentinel, since we need to do declaration and move on same line.
        auto& sentinel = MemorySentinel::getInstance();
        MemorySentinel::setTransgressionBehaviour(MemorySentinel::TransgressionBehaviour::SILENT);
        sentinel.clearTransgressions();
        sentinel.setArmed(true);
        
        HyperBuffer<int, N> bufferMovedTo = std::move(bufferMovedFrom);
        
        REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());
        sentinel.setArmed(false);
        verifyBuffer(bufferMovedTo);
        REQUIRE(bufferMovedTo.data() != nullptr);
    }
#endif
    
    // Move Ctor
    {
        HyperBuffer<int, N> bufferMovedFrom = buffer;
        HyperBuffer<int, N> bufferMovedToCtor(std::move(bufferMovedFrom));
        verifyBuffer(bufferMovedToCtor);
        REQUIRE(bufferMovedToCtor.data() != nullptr);
    }
    
#if MSVC_DEBUG == 0
    // verify no memory is allocated during move construction
    {
        HyperBuffer<int, N> bufferMovedFrom = buffer; // working copy
        
        // Can't use Scoped Sentinel, since we need to do declaration and move on same line.
        auto& sentinel = MemorySentinel::getInstance();
        MemorySentinel::setTransgressionBehaviour(MemorySentinel::TransgressionBehaviour::SILENT);
        sentinel.clearTransgressions();
        sentinel.setArmed(true);
        
        HyperBuffer<int, N> bufferMovedCtor(std::move(bufferMovedFrom));
        REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());
        
        sentinel.setArmed(false);
        REQUIRE(bufferMovedCtor.data() != nullptr);
    }
#endif
    
}

TEST_CASE("Copy/Move a HyperBuffer with external, flat allocation")
{
    constexpr int N = 3;
    int preAllocData[3*2*8] {0};
    std::array<int, 3> dims {3, 2, 8};
    HyperBufferView<int, N> buffer(preAllocData, dims);
    buffer[1][0][5] = 333;
    buffer[2][1][3] = -666;

    // Copy assignment operator
    HyperBufferView<int, N> bufferCopy = buffer;
    verifyBuffer(bufferCopy);
    verifyBuffer(buffer); // original remains untouched
    REQUIRE(bufferCopy[0] == buffer[0]); // copy points to the same data
    REQUIRE(bufferCopy[0][1] == buffer[0][1]);
    REQUIRE(bufferCopy[2][0] == buffer[2][0]);

    // Copy Ctor - verify no memory is allocated
    HyperBufferView<int, N> bufferCopyCtor(preAllocData, 3, 2, 8);
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
        HyperBufferView<int, N> bufferMovedFrom = buffer; // working copy
        HyperBufferView<int, N> bufferMovedTo(preAllocData, 3, 2, 8);
        bufferMovedTo = std::move(bufferMovedFrom);
        verifyBuffer(bufferMovedTo);
        //REQUIRE(bufferMovedFrom.data() == nullptr); // this cannot be relied upon
        REQUIRE(bufferMovedTo[0] == buffer[0]); // moved points to the same data
        REQUIRE(bufferMovedTo[0][1] == buffer[0][1]);
        REQUIRE(bufferMovedTo[2][0] == buffer[2][0]);
    }

#if MSVC_DEBUG == 0
    // verify no memory is allocated during move assignment
    {
        HyperBufferView<int, N> bufferMovedFrom = buffer; // working copy

        // Can't use Scoped Sentinel, since we need to do declaration and move on same line.
        auto& sentinel = MemorySentinel::getInstance();
        MemorySentinel::setTransgressionBehaviour(MemorySentinel::TransgressionBehaviour::SILENT);
        sentinel.clearTransgressions();
        sentinel.setArmed(true);
        
        HyperBufferView<int, N> bufferMovedTo = std::move(bufferMovedFrom);
        
        REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());
        sentinel.setArmed(false);
        verifyBuffer(bufferMovedTo);
        REQUIRE(bufferMovedTo.data() != nullptr);
        //REQUIRE(bufferMovedFrom.data() == nullptr); // this cannot be relied upon
    }
#endif
    
    // Move Ctor
    {
        HyperBufferView<int, N> bufferMovedFrom = buffer;
        HyperBufferView<int, N> bufferMovedToCtor(std::move(bufferMovedFrom));
        verifyBuffer(bufferMovedToCtor);
        REQUIRE(bufferMovedFrom.data() == nullptr);
        REQUIRE(bufferMovedToCtor[0] == buffer[0]); // moved points to the same data
        REQUIRE(bufferMovedToCtor[0][1] == buffer[0][1]);
        REQUIRE(bufferMovedToCtor[2][0] == buffer[2][0]);
    }
    
#if MSVC_DEBUG == 0
    // verify no memory is allocated during move construction
    {
        HyperBufferView<int, N> bufferMovedFrom = buffer; // working copy
        
        // Can't use Scoped Sentinel, since we need to do declaration and move on same line.
        auto& sentinel = MemorySentinel::getInstance();
        MemorySentinel::setTransgressionBehaviour(MemorySentinel::TransgressionBehaviour::SILENT);
        sentinel.clearTransgressions();
        sentinel.setArmed(true);
        
        HyperBufferView<int, N> bufferMovedCtor(std::move(bufferMovedFrom));
        
        REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());
        sentinel.setArmed(false);
        verifyBuffer(bufferMovedCtor);
        REQUIRE(bufferMovedCtor.data() != nullptr);
        //REQUIRE(bufferMovedFrom.data() == nullptr);  // this cannot be relied upon
    }
#endif
    
}

TEST_CASE("Copy/Move a HyperBuffer with external, multi-dimensional allocation")
{
    constexpr int N = 3;
    std::array<int, 3> dims {3, 2, 8};
    int preAllocData1[8] {0};
    int preAllocData2[8] {0};
    int* dim1_0[] { preAllocData1, preAllocData2 };
    int* dim1_1[] { preAllocData1, preAllocData2 };
    int* dim1_2[] { preAllocData1, preAllocData2 };
    int** preallocData[] { dim1_0, dim1_1, dim1_2 };

    // Construction does not allocate memory
    {
        ScopedMemorySentinel sentinel;
        HyperBufferViewNC<int, N> buffer(preallocData, dims);
    }
    HyperBufferViewNC<int, N> buffer(preallocData, dims);
    buffer[1][0][5] = 333;
    buffer[2][1][3] = -666;

    // Copy assignment operator
    {
        ScopedMemorySentinel sentinel;
        HyperBufferViewNC<int, N> bufferCopy = buffer;
        UNUSED(bufferCopy);
    }
    HyperBufferViewNC<int, N> bufferCopy = buffer;
    verifyBuffer(bufferCopy);
    verifyBuffer(buffer); // original remains untouched
    REQUIRE(bufferCopy[0] == buffer[0]); // copy points to the same data
    REQUIRE(bufferCopy[0][1] == buffer[0][1]);
    REQUIRE(bufferCopy[2][0] == buffer[2][0]);

    // Copy Ctor - verify no memory is allocated
    {
        HyperBufferViewNC<int, N> bufferCopyCtor(nullptr, 3, 2, 8);
        {
            ScopedMemorySentinel sentinel;
            bufferCopyCtor = buffer;
        }
        verifyBuffer(bufferCopyCtor);
        verifyBuffer(buffer); // original remains untouched
        REQUIRE(bufferCopy[0] == buffer[0]); // copy points to the same data
        REQUIRE(bufferCopy[0][1] == buffer[0][1]);
        REQUIRE(bufferCopy[2][0] == buffer[2][0]);
    }

    // Move assignment operator
    {
        HyperBufferViewNC<int, N> bufferMovedFrom = buffer; // working copy
        HyperBufferViewNC<int, N> bufferMovedTo = std::move(bufferMovedFrom);
        verifyBuffer(bufferMovedTo);
        //REQUIRE(bufferMovedFrom.data() != nullptr); // original remains untouched -- NOTE: this cannot be relied upon
        REQUIRE(bufferMovedTo[0] == buffer[0]); // moved points to the same data
        REQUIRE(bufferMovedTo[0][1] == buffer[0][1]);
        REQUIRE(bufferMovedTo[2][0] == buffer[2][0]);
    }

    // verify no memory is allocated during move assignment
    {
        HyperBufferViewNC<int, N> bufferMovedFrom = buffer; // working copy
        HyperBufferViewNC<int, N> bufferMovedToPreAlloc(nullptr, dims);
        {
            ScopedMemorySentinel sentinel;
            bufferMovedToPreAlloc = std::move(bufferMovedFrom);
        }
        verifyBuffer(bufferMovedToPreAlloc);
        //REQUIRE(bufferMovedFrom.data() != nullptr); // original remains untouched -- NOTE: this cannot be relied upon
    }

    // Move Ctor
    {
        HyperBufferViewNC<int, N> bufferMovedFrom = buffer;
        HyperBufferViewNC<int, N> bufferMovedToCtor(std::move(bufferMovedFrom));
        verifyBuffer(bufferMovedToCtor);
        //REQUIRE(bufferMovedFrom.data() != nullptr); // original remains untouched -- NOTE: this cannot be relied upon
        REQUIRE(bufferMovedToCtor[0] == buffer[0]); // moved points to the same data
        REQUIRE(bufferMovedToCtor[0][1] == buffer[0][1]);
        REQUIRE(bufferMovedToCtor[2][0] == buffer[2][0]);
    }

    // verify no memory is allocated during move construction
    {
        HyperBufferViewNC<int, N> bufferMovedFrom = buffer; // working copy
        {
            ScopedMemorySentinel sentinel;
            HyperBufferViewNC<int, N> bufferMovedCtor(std::move(bufferMovedFrom));
            UNUSED(bufferMovedCtor);
        }
        //REQUIRE(bufferMovedFrom.data() != nullptr); // original remains untouched -- NOTE: this cannot be relied upon
    }
}

