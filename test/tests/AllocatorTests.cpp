//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include "TestCommon.hpp"

#include "Allocator.hpp"
#include "MemorySentinel.hpp"

TEST_CASE("Custom Allocator Tests")
{
    using MyVector = std::vector<int, Allocator<int>>;
    
    auto& sentinel = MemorySentinel::getInstance();
    sentinel.setTransgressionBehaviour(MemorySentinel::TransgressionBehaviour::SILENT);
    sentinel.clearTransgressions();
    sentinel.setArmed(true);
    
    MyVector myVector(8); // allocate 8
    REQUIRE(sentinel.getAndClearTransgressionsOccured());

    myVector[7] = 3;
    myVector.resize(32); // allocate 32, deallocate 8
    REQUIRE(sentinel.getAndClearTransgressionsOccured());
    
    // No de-/allocation is triggered!
    MyVector myVectorMovedTo = std::move(myVector);
    REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());

    myVectorMovedTo.clear(); // this does not deallocate!
    REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());

    // deallocate all stack variables
    sentinel.setArmed(false);
}
