//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <catch2/catch.hpp>

#include <array>
#include <functional>

#include "IntArrayOperations.hpp"


TEST_CASE("IntArrayOperations Tests")
{
    // NOTE: the 'guts' of these functions are all calculated by VarArg functions, which are more thoroughly tested.
    // this just verifies parameters are passed correctly.
    using namespace IntArrayOperations;
    
    REQUIRE(sum({3, 1, 2, 5}) == 11);
    REQUIRE(sumCapped(1, {3, 2}) == 3);
    REQUIRE(sumCapped(2, {2, 7}) == 9);
    REQUIRE(sumCapped(2, {1, 2, 8}) == 3);

    REQUIRE(sumOverRange(1, 4, {3, 1, 2, 5}) == 8);
    REQUIRE(sumOverRange(0, 4, {3, 1, 2, 5}) == 11);
    REQUIRE(sumOverRange(0, 3, {3, 1, 4, 5}) == 8);
   
    REQUIRE(product({2, 3}) == 6);
    REQUIRE(productCapped(2, {1, 2, 0}) == 2);
    REQUIRE(productCapped(1, {2, 2}) == 2);
    REQUIRE(productCapped(2, {2, 7}) == 14);
    REQUIRE(productCapped(2, {1, 2, 0}) == 2);

    REQUIRE(productOverRange(1, 4, {3, 1, 2, 5}) == 10);
    REQUIRE(productOverRange(0, 4, {3, 1, 2, 5}) == 30);
    REQUIRE(productOverRange(0, 3, {3, 1, 2, 5}) == 6);
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

    REQUIRE(sumOverRange(1, 4, std::array<int, 4>{3, 1, 2, 5}) == 8);
    REQUIRE(sumOverRange(0, 4, std::array<int, 4>{3, 1, 2, 5}) == 11);
    REQUIRE(sumOverRange(0, 3, std::array<int, 4>{3, 1, 4, 5}) == 8);
   
    REQUIRE(product(std::array<int, 2>{2, 3}) == 6);
    REQUIRE(productCapped(2, std::array<int, 3>{1, 2, 0}) == 2);
    REQUIRE(productCapped(1, std::array<int, 2>{2, 2}) == 2);
    REQUIRE(productCapped(2, std::array<int, 2>{2, 7}) == 14);
    REQUIRE(productCapped(2, std::array<int, 3>{1, 2, 0}) == 2);

    REQUIRE(productOverRange(1, 4, std::array<int, 4>{3, 1, 2, 5}) == 10);
    REQUIRE(productOverRange(0, 4, std::array<int, 4>{3, 1, 2, 5}) == 30);
    REQUIRE(productOverRange(0, 3, std::array<int, 4>{3, 1, 2, 5}) == 6);
}
