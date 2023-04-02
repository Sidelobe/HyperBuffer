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
    #include "TemplateUtils.hpp"
    #include "CompiletimeMath.hpp"
#endif

using namespace slb;

TEST_CASE("TemplateUtils Assertion Tests")
{
    ASSERT(true);
    REQUIRE_THROWS(ASSERT(false));
}

TEST_CASE("TemplateUtils VarArgOperations apply() Tests")
{
    std::array<int, 3> array { 2, 3, 4 };
    
    // first way of using it
    REQUIRE(VarArgOperations::apply(CompiletimeMath::sum<int, int, int>, array) == 9);
    
    // second way of using it
    REQUIRE(VarArgOperations::apply( [](auto&&... args)
    {
        return CompiletimeMath::sum(std::forward<decltype(args)>(args)...);
    }, array) == 9);
}

template<typename... I>
static constexpr int constExprFunction(I... i)
{
    return CompiletimeMath::sumOfCumulativeProduct(i...);
}

TEST_CASE("TemplateUtils VarArgOperations are constexpr")
{
    // this should be assignable to a constexpr
    constexpr int a = CompiletimeMath::sum(1, 2);
    static_assert(a == 3, "");
    constexpr int b = CompiletimeMath::product(1, 2);
    static_assert(b == 2, "");
    constexpr int c = CompiletimeMath::sumCapped(2, 1, 2, 3);
    static_assert(c == 3, "");
    constexpr int d = CompiletimeMath::sumOfCumulativeProduct(1, 2, 3);
    static_assert(d == 9, "");

    constexpr int e = constExprFunction(1, 2, 3);
    static_assert(e == 9, "");
}


