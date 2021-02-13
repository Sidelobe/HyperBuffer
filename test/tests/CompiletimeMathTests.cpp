//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include "TestCommon.hpp"

#include <array>

#include "CompiletimeMath.hpp"

using namespace slb;

TEST_CASE("CompiletimeMath Tests")
{
    using namespace CompiletimeMath;
    
    // These functions are evaluated at compile-time, therefore we do static_asserts instead of run-time tests.
    // They are written in catch2 style for convenience only.
    
    SECTION("isEveryElementLargerThanZero") {
        static_assert(areAllPositive(1) == true, "");
        static_assert(areAllPositive(-1) == false, "");
        static_assert(areAllPositive(0) == false, "");
        static_assert(areAllPositive(1, 2, 99) == true, "");
        static_assert(areAllPositive(1, 2, -99) == false, "");
        static_assert(areAllPositive(0, 0) == false, "");
    }
    
    SECTION("sum") {
        static_assert(sum(1) == 1, "");
        static_assert(sum(0) == 0, "");
        static_assert(sum(1, 1) == 2, "");
        static_assert(sum(1, 0) == 1, "");
        static_assert(sum(2, 2) == 4, "");
        static_assert(sum(2, 2, 1) == 5, "");
        static_assert(sum(2, 2, 3, 3) == 10, "");
        static_assert(sum(2, 2, 3, 0) == 7, "");
        
        static_assert(sum(1, -1) == 0, "");
        static_assert(sum(2.f, 1.f) == 3.f, "");
        static_assert((sum(2.f, .5f) - 2.5f) < 1e-9f, ""); // cannot assert sum(2.f, .5f) = 2.5f due to floating point
    }
    
    SECTION("sumCapped and sumOverRange") {
        static_assert(sumCapped(2, 2, 2, 3, 0) == 4, "");
        static_assert(sumCapped(0, 2, 2, 2, 3) == 0, ""); // 0 cap index
        static_assert(sumOverRange(2, 3, 2, 2, 3, -4) == 1, "");
        static_assert(sumOverRange(1, 4, 2, 2, 3, -4) == 3, "");
        static_assert(sumOverRange(1, 0, 2, 2, 3) == 0, "zero summands");
        static_assert(sumOverRange(2, -1, 2, 2, 3) == 0, "negative num summands");
        static_assert(sumOverRange(-1, 1, 2, 2, 3) == 2, "negative range start");
        static_assert(sumOverRange(0, 4, 2, 2, 3) == sumOverRange(1, 3, 2, 2, 3), "num summands too high - absolute");
        static_assert(sumOverRange(2, 3, 2, 2, 3) == sumOverRange(2, 2, 2, 2, 3), "num summands too high - relative");
    }
    
    SECTION("product") {
        static_assert(product(1) == 1, "");
        static_assert(product(0) == 0, "");
        static_assert(product(1, 1) == 1, "");
        static_assert(product(1, 0) == 0, "");
        static_assert(product(2, 2) == 4, "");
        static_assert(product(2, 2, 1) == 4, "");
        static_assert(product(2, 2, 3, 3) == 36, "");
        static_assert(product(2, 2, 3, 0) == 0, "");
        
        static_assert(product(2, -1) == -2, "");
        static_assert(product(2.f, 1.f) == 2.f, "");
        static_assert((product(2.f, .5f) - 1.0f) < 1e-12f, "");
    }
    
    SECTION("productCapped") {
        static_assert(productCapped(1, 1) == 1, "");
        static_assert(productCapped(1, 0) == 0, "");
        static_assert(productCapped(1, 1, 1) == 1, "");
        static_assert(productCapped(2, 1, 0) == 0, "");
        static_assert(productCapped(1, 2, 2) == 2, "");
        static_assert(productCapped(2, 2, 2, 1) == 4, "");
        static_assert(productCapped(3, 2, 2, 3, 3) == 12, "");
        static_assert(productCapped(4, 2, 2, 3, 0) == 0, "");
        
        static_assert(productOverRange(2, 3, 2, 2, 3, -4) == -24, "");
        static_assert(productOverRange(1, 4, 2, 2, 3, -4) == -48, "");
        static_assert(productOverRange(1, 0, 2, 2, 3) == 0, "zero factors");
        static_assert(productOverRange(1, -1, 2, 2, 3) == 0, "neg num factors");
        static_assert(productOverRange(-1, 3, 2, 2, 3) == 12, "negative range start");
        static_assert(productOverRange(0, 4, 2, 2, 3) == productOverRange(1, 3, 2, 2, 3), "num factors too high - absolute");
        static_assert(productOverRange(2, 3, 2, 2, 3) == productOverRange(2, 2, 2, 2, 3), "num factors too high - relative");
    }
    
    SECTION("sumOfCumulativeProduct") {
        static_assert(sumOfCumulativeProduct(0) == 0, "");
        static_assert(sumOfCumulativeProduct(1) == 1, "");
        
        static_assert(sumOfCumulativeProduct(1, 1) == 2, "");
        static_assert(sumOfCumulativeProduct(2, 0) == 2, "");
        static_assert(sumOfCumulativeProduct(2, 2) == 6, "");
        static_assert(sumOfCumulativeProduct(2, 2, 1) == 10, "");
        static_assert(sumOfCumulativeProduct(1, 3, 2) == 10, "");
        static_assert(sumOfCumulativeProduct(2, 2, 3, 3) == 54, "");
        static_assert(sumOfCumulativeProduct(2, 2, 3, 0) == 18, "");
        
        static_assert(sumOfCumulativeProduct(2, -1) == 0, "");
        static_assert(sumOfCumulativeProduct(2.f, 1.f) == 4.f, "");
        static_assert(sumOfCumulativeProduct(2.f, .5f) == 3.f, "");
    }
    
    SECTION("sumOfCumulativeProduct: limited range / cap") {
        {
            constexpr int cap = 1;
            static_assert(sumOfCumulativeProductCapped(cap, 0) == 0, "");
            static_assert(sumOfCumulativeProductCapped(cap, 1) == 1, "");
            static_assert(sumOfCumulativeProductCapped(cap, 2, 2) == 2, "");
        }
        {
            constexpr int cap = 2;
            static_assert(sumOfCumulativeProductCapped(cap, 1, 1) == 2, "");
            static_assert(sumOfCumulativeProductCapped(cap, 2, 0) == 2, "");
            static_assert(sumOfCumulativeProductCapped(cap, 2, 2, 1) == 6, "");
        }
        {
            constexpr int cap = 3;
            static_assert(sumOfCumulativeProductCapped(cap, 1, 3, 2) == 10, "");
            static_assert(sumOfCumulativeProductCapped(cap, 2, 2, 3, 3) == 18, "");
            static_assert(sumOfCumulativeProductCapped(cap, 2, 2, 3, 0) == 18, "");
        }
        
        static_assert(sumOfCumulativeProductOverRange(1, 3, 2, 2, 3) == 18, "");
        static_assert(sumOfCumulativeProductOverRange(1, 2, 2, 2, 3) == 6, "");
        static_assert(sumOfCumulativeProductOverRange(2, 2, 2, 2, 3) == 16, "");
        static_assert(sumOfCumulativeProductOverRange(1, 0, 2, 2, 3) == 0, "zero elements");
        static_assert(sumOfCumulativeProductOverRange(1, -1, 2, 2, 3) == 0, "neg num elements");
        static_assert(sumOfCumulativeProductOverRange(-1, 3, 2, 2, 3) == 18, "negative range start");
        static_assert(sumOfCumulativeProductOverRange(0, 4, 2, 2, 3) == sumOfCumulativeProductOverRange(1, 3, 2, 2, 3), "num elements too high - absolute");
        static_assert(sumOfCumulativeProductOverRange(2, 3, 2, 2, 3) == sumOfCumulativeProductOverRange(2, 2, 2, 2, 3), "num elements too high - relative");
    }
    
    // These run-time test were added solely to satisfy code coverage metrics (traverse every branch)
    
    REQUIRE(areAllPositive(1));
    REQUIRE_FALSE(areAllPositive(0));
    REQUIRE_FALSE(areAllPositive(-1));
    REQUIRE(areAllPositive(1, 1));
    REQUIRE_FALSE(areAllPositive(0, 1));
    REQUIRE_FALSE(areAllPositive(1, -1));
    REQUIRE_FALSE(areAllPositive(1, 2, -3));
    REQUIRE(areAllPositive(1, 3, 6));
    REQUIRE_FALSE(areAllPositive(-1, 3, 6));

    REQUIRE(sumOverRange(1, 0, 2, 2, 3) == 0); // zero summands
    REQUIRE(sumOverRange(2, -1, 2, 2, 3) == 0); // negative num summands
    REQUIRE(sumOverRange(-1, 1, 2, 2, 3) == 2); // negative range start
    REQUIRE(sumOverRange(0, 0, 1) == 0);
    REQUIRE(sumOverRange(1, 4, 1) == 1);

    REQUIRE(productOverRange(1, 0, 2, 2, 3) == 0); // zero summands
    REQUIRE(productOverRange(2, -1, 2, 2, 3) == 0); // negative num summands
    REQUIRE(productOverRange(-1, 1, 2, 2, 3) == 2); // negative range start
    REQUIRE(productOverRange(0, 0, 1) == 0);
    REQUIRE(productOverRange(0, -1, 1) == 0);
    
    REQUIRE(sumOfCumulativeProductOverRange(0, 0, 1) == 0);
    REQUIRE(sumOfCumulativeProductOverRange(-1, 0, 1) == 0);
    REQUIRE(sumOfCumulativeProductOverRange(-1, -1, 1) == 0);

}
