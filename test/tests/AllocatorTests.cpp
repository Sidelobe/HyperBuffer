//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include "TestCommon.hpp"

#include "Allocator.hpp"
#include "MemorySentinel.hpp"

template<class T>
class TestClass
{
public:
    TestClass(int size) : m_vectorMember(size) {}

private:
    std::vector<T> m_vectorMember;
};

TEST_CASE("Standard Allocator Tests")
{
    auto& sentinel = MemorySentinel::getInstance();
    sentinel.setTransgressionBehaviour(MemorySentinel::TransgressionBehaviour::SILENT);
    sentinel.clearTransgressions();
    sentinel.setArmed(true);
    
    std::vector<int*> myVector(8); // allocate 8
    REQUIRE(sentinel.getAndClearTransgressionsOccured());

    myVector[7] = nullptr;
    myVector.resize(32); // allocate 32, deallocate 8
    REQUIRE(sentinel.getAndClearTransgressionsOccured());
    
    // No de-/allocation is triggered!
    std::vector<int*> myVectorMovedTo = std::move(myVector);
    REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());

    myVectorMovedTo.clear(); // this does not deallocate!
    REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());

    TestClass<int> myTestClass(8);
    REQUIRE(sentinel.getAndClearTransgressionsOccured());
    
    TestClass<int> myTestClassMovedTo = std::move(myTestClass);
    REQUIRE_FALSE(sentinel.getAndClearTransgressionsOccured());

    // deallocates all stack variables when exiting scope...
    sentinel.setArmed(false);
}

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
