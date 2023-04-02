//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include "TestCommon.hpp"

#include <array>
#include <functional>

#ifdef SLB_AMALGATED_HEADER
    #include "HyperBuffer.hpp"
#else
    #include "IntArrayOperations.hpp"
#endif

using namespace slb;

TEST_CASE("StdArrayOperations Tests")
{
    // NOTE: the 'guts' of these functions are all calculated by VarArg functions, which are more thoroughly tested.
    // this just verifies parameters are passed correctly.
    using namespace StdArrayOperations;
    
    REQUIRE(product(std::array<int, 2>{2, 3}) == 6);
    REQUIRE(productCapped(2, std::array<int, 3>{1, 2, 0}) == 2);
    REQUIRE(productCapped(1, std::array<int, 2>{2, 2}) == 2);
    REQUIRE(productCapped(2, std::array<int, 2>{2, 7}) == 14);
    REQUIRE(productCapped(2, std::array<int, 3>{1, 2, 0}) == 2);

    REQUIRE(sumOfCumulativeProductCapped(3, std::array<int, 4>{2, 3, 2, 3}) == 20);

    REQUIRE(shaveOffFirstElement(std::array<int, 4>{6, 5, 2, 3}) == std::array<int, 3>{5, 2, 3});
    REQUIRE(shaveOffFirstElement(std::array<int, 2>{5, 3}) == std::array<int, 1>{3});
}
