//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <catch2/catch.hpp>

#include <array>
#include <functional>

#include "TemplateUtils.hpp"

TEST_CASE("TemplateUtils VarArgOperations Tests")
{
    using namespace VarArgOperations;
    
    // These functions are evaluated at compile-time, therefore we do static_asserts instead of run-time tests.
    // They are written in catch2 style for convenience only.
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
        static_assert((sum(2.f, .5f) - 2.5f) < 1e-9, ""); // cannot assert sum(2.f, .5f) = 2.5f due to floating point
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
        static_assert(product(2.f, .5f) == 1.f, "");
    }
    
    SECTION("productCapped") {
        static_assert(productCapped(1, 1) == 1, "");
        static_assert(product(0) == 0, "");
        static_assert(product(1, 1) == 1, "");
        static_assert(product(1, 0) == 0, "");
        static_assert(product(2, 2) == 4, "");
        static_assert(product(2, 2, 1) == 4, "");
        static_assert(product(2, 2, 3, 3) == 36, "");
        static_assert(product(2, 2, 3, 0) == 0, "");
        
        static_assert(product(2, -1) == -2, "");
        static_assert(product(2.f, 1.f) == 2.f, "");
        static_assert(product(2.f, .5f) == 1.f, "");
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
    }
}

TEST_CASE("TemplateUtils VarArgOperations apply() Tests")
{
    std::array<int, 3> array { 2, 3, 4 };
    
    // first way of using it
    REQUIRE(VarArgOperations::apply(VarArgOperations::sum<int, int, int>, array) == 9);
    
    // second way of using it
    REQUIRE(VarArgOperations::apply( [](auto&&... args)
    {
        return VarArgOperations::sum(std::forward<decltype(args)>(args)...);
    }, array) == 9);
}

template<typename... I>
static constexpr int constExprFunction(I... i)
{
    return VarArgOperations::sumOfCumulativeProduct(i...);
};

TEST_CASE("TemplateUtils VarArgOperations are constexpr")
{
    // this should be assignable to a constexpr
    constexpr int a = VarArgOperations::sum(1, 2);
    static_assert(a == 3, "");
    constexpr int b = VarArgOperations::product(1, 2);
    static_assert(b == 2, "");
    constexpr int c = VarArgOperations::sumCapped(2, 1, 2, 3);
    static_assert(c == 3, "");
    constexpr int d = VarArgOperations::sumOfCumulativeProduct(1, 2, 3);
    static_assert(d == 9, "");

    constexpr int e = constExprFunction(1, 2, 3);
    static_assert(e == 9, "");
}


