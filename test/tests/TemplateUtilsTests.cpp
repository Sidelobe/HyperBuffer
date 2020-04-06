
#include <catch2/catch.hpp>

#include "TemplateUtils.hpp"

TEST_CASE("TemplateUtils Tests")
{
    SECTION("MultiplyArgs") {
        REQUIRE(multiplyArgs(1) == 1);
        REQUIRE(multiplyArgs(0) == 0);
        REQUIRE(multiplyArgs(1, 1) == 1);
        REQUIRE(multiplyArgs(1, 0) == 0);
        REQUIRE(multiplyArgs(2, 2) == 4);
        REQUIRE(multiplyArgs(2, 2, 1) == 4);
        REQUIRE(multiplyArgs(2, 2, 3, 3) == 36);
        REQUIRE(multiplyArgs(2, 2, 3, 0) == 0);
        
        REQUIRE(multiplyArgs(2.f, 1.f) == 2.f);
        REQUIRE(multiplyArgs(2.f, .5f) == Approx(1.f));
    }
    
}
