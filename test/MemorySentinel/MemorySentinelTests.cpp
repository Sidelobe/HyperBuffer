
#include <catch2/catch.hpp>

#include "MemorySentinel.hpp"

#include <vector>

static decltype(auto) allocWithNew()
{
    return new std::vector<float>(32);
}
static decltype(auto) allocWithNewArray()
{
    return new float[32];
}

static decltype(auto) allocWithMalloc()
{
    return std::malloc(32*sizeof(float));
}

TEST_CASE("MemorySentinel Tests")
{
    MemorySentinel& sentinel = MemorySentinel::getInstance();
 
    sentinel.setTransgressionBehaviour(MemorySentinel::TransgressionBehaviour::SILENT);
    MemorySentinel::getInstance().setArmed(false);
    REQUIRE_FALSE(MemorySentinel::getInstance().isArmed());
    std::vector<float>* heapObject = allocWithNew();
    REQUIRE(heapObject != nullptr);
    REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());
    delete heapObject; // clean up

    MemorySentinel::getInstance().setArmed(true);
    REQUIRE(MemorySentinel::getInstance().isArmed());
    heapObject = allocWithNew();
    REQUIRE(heapObject != nullptr);
    REQUIRE(sentinel.getAndClearTransgressionsOccured());
    delete heapObject; // clean up
    
    sentinel.setTransgressionBehaviour(MemorySentinel::TransgressionBehaviour::THROW_EXCEPTION);
    MemorySentinel::getInstance().setArmed(true);
    REQUIRE(MemorySentinel::getInstance().isArmed());
    REQUIRE_THROWS(allocWithNew());
    REQUIRE(heapObject != nullptr);
    REQUIRE(sentinel.getAndClearTransgressionsOccured());
    // clean-up not necessary, since allocation was intercepted by exception

    MemorySentinel::getInstance().setArmed(true);
    REQUIRE(MemorySentinel::getInstance().isArmed());
    REQUIRE_THROWS(allocWithNewArray());
    REQUIRE(heapObject != nullptr);
    REQUIRE(sentinel.getAndClearTransgressionsOccured());
    // clean-up not necessary, since allocation was intercepted by exception
    
    MemorySentinel::getInstance().setArmed(true);
    REQUIRE(MemorySentinel::getInstance().isArmed());
    REQUIRE_THROWS(allocWithMalloc());
    REQUIRE(heapObject != nullptr);
    REQUIRE(sentinel.getAndClearTransgressionsOccured());
    // clean-up not necessary, since allocation was intercepted by exception
    
    // After tests, disarm Sentinel
    MemorySentinel::getInstance().setArmed(false);
    sentinel.clearTransgressions();
}

TEST_CASE("ScopedMemorySentinel Tests")
{
    {
        ScopedMemorySentinel sentinel;
        // THIS WILL ASSERT
        //std::vector<float>* heapObject = allocWithNew();
    }
}
