//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include "TestCommon.hpp"

#include <array>
#include <functional>

#include "IntArrayOperations.hpp"

using namespace slb;

TEST_CASE("IntArrayOperations Tests")
{
    // NOTE: the 'guts' of these functions are all calculated by VarArg functions, which are more thoroughly tested.
    // this just verifies parameters are passed correctly.
    using namespace IntArrayOperations;
    
    REQUIRE(sum({3, 1, 2, 5}) == 11);
    REQUIRE(sumCapped(1, {3, 2}) == 3);
    REQUIRE(sumCapped(2, {2, 7}) == 9);
    REQUIRE(sumCapped(2, {1, 2, 8}) == 3);
    REQUIRE(sumOverRange(2, 4, {3, 1, 2, 5}) == 8);
    REQUIRE(sumOverRange(1, 4, {3, 1, 2, 5}) == 11);
    REQUIRE(sumOverRange(1, 3, {3, 1, 4, 5}) == 8);
   
    REQUIRE(product({2, 3}) == 6);
    REQUIRE(productCapped(2, {1, 2, 0}) == 2);
    REQUIRE(productCapped(1, {2, 2}) == 2);
    REQUIRE(productCapped(2, {2, 7}) == 14);
    REQUIRE(productCapped(2, {1, 2, 0}) == 2);

    REQUIRE(productOverRange(2, 4, {3, 1, 2, 5}) == 10);
    REQUIRE(productOverRange(1, 4, {3, 1, 2, 5}) == 30);
    REQUIRE(productOverRange(1, 3, {3, 1, 2, 5}) == 6);
    REQUIRE(productOverRange(-1, 3, {3, 1, 2, 5}) == 6);
    REQUIRE(productOverRange(0, -1, {3, 1, 2, 5}) == 0);
    REQUIRE(productOverRange(-3, -1, {3, 1, 2, 5}) == 0);
}

TEST_CASE("StdArrayOperations Tests")
{
    // NOTE: the 'guts' of these functions are all calculated by VarArg functions, which are more thoroughly tested.
    // this just verifies parameters are passed correctly.
    using namespace StdArrayOperations;
    
    REQUIRE(sum(std::array<int, 4>{3, 1, 2, 5}) == 11);
    REQUIRE(sumCapped(1, std::array<int, 2>{3, 2}) == 3);
    REQUIRE(sumCapped(2, std::array<int, 2>{2, 7}) == 9);
    REQUIRE(sumCapped(2, std::array<int, 3>{1, 2, 8}) == 3);

    REQUIRE(sumOverRange(2, 4, std::array<int, 4>{3, 1, 2, 5}) == 8);
    REQUIRE(sumOverRange(1, 4, std::array<int, 4>{3, 1, 2, 5}) == 11);
    REQUIRE(sumOverRange(1, 3, std::array<int, 4>{3, 1, 4, 5}) == 8);
   
    REQUIRE(product(std::array<int, 2>{2, 3}) == 6);
    REQUIRE(productCapped(2, std::array<int, 3>{1, 2, 0}) == 2);
    REQUIRE(productCapped(1, std::array<int, 2>{2, 2}) == 2);
    REQUIRE(productCapped(2, std::array<int, 2>{2, 7}) == 14);
    REQUIRE(productCapped(2, std::array<int, 3>{1, 2, 0}) == 2);

    REQUIRE(productOverRange(2, 4, std::array<int, 4>{3, 1, 2, 5}) == 10);
    REQUIRE(productOverRange(1, 4, std::array<int, 4>{3, 1, 2, 5}) == 30);
    REQUIRE(productOverRange(1, 3, std::array<int, 4>{3, 1, 2, 5}) == 6);
    REQUIRE(productOverRange(1, 0, std::array<int, 4>{3, 1, 2, 5}) == 0);
    REQUIRE(productOverRange(-1, 3, std::array<int, 4>{3, 1, 2, 5}) == 6);
    REQUIRE(productOverRange(1, -1, std::array<int, 4>{3, 1, 2, 5}) == 0);
    REQUIRE(productOverRange(-3, -1, std::array<int, 4>{3, 1, 2, 5}) == 0);
    
    REQUIRE(sumOfCumulativeProduct(std::array<int, 4>{2, 3, 2, 3}) == 56);
    REQUIRE(sumOfCumulativeProductCapped(3, std::array<int, 4>{2, 3, 2, 3}) == 20);

    REQUIRE(shaveOffFirstElement(std::array<int, 4>{6, 5, 2, 3}) == std::array<int, 3>{5, 2, 3});
    REQUIRE(shaveOffFirstElement(std::array<int, 2>{5, 3}) == std::array<int, 1>{3});
}
