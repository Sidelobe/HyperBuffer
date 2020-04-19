//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <catch2/catch.hpp>

#include <vector>

#include "BufferGeometry.hpp"

TEST_CASE("BufferGeometry Tests")
{
    SECTION("1D") {
        constexpr int N = 1;
        BufferGeometry<N> bufferGeo(3);
        CHECK(bufferGeo.getOffsetInDataArray(0) == 0);
        CHECK(bufferGeo.getOffsetInDataArray(1) == 1);
        CHECK(bufferGeo.getOffsetInDataArray(2) == 2);
    }
    
    SECTION("2D") {
        constexpr int N = 2;
        BufferGeometry<N> bufferGeo(3, 3);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0) == 0);
        CHECK(bufferGeo.getOffsetInDataArray(0, 1) == 1);
        CHECK(bufferGeo.getOffsetInDataArray(0, 2) == 2);
        CHECK(bufferGeo.getOffsetInDataArray(1, 0) == 3);
        CHECK(bufferGeo.getOffsetInDataArray(1, 1) == 4);
        CHECK(bufferGeo.getOffsetInDataArray(1, 2) == 5);
        CHECK(bufferGeo.getOffsetInDataArray(2, 0) == 6);
        CHECK(bufferGeo.getOffsetInDataArray(2, 1) == 7);
        CHECK(bufferGeo.getOffsetInDataArray(2, 2) == 8);

        CHECK(bufferGeo.getOffsetInPointerArray<0>(0) == 0);
        CHECK(bufferGeo.getOffsetInPointerArray<0>(1) == 1);
        CHECK(bufferGeo.getOffsetInPointerArray<0>(2) == 2);

        float data [3*3] {0};
        constexpr int pointerDimensions = N-1;
        float* pointers [VarArgOperations::sumOfCumulativeProductCapped(pointerDimensions, 3, 3)];
        REQUIRE(getRawArrayLength(pointers) == 3);
        bufferGeo.hookupPointerArrayToData(data, pointers);
        REQUIRE(pointers[0] == &data[bufferGeo.getOffsetInDataArray(0, 0)]);
        REQUIRE(pointers[1] == &data[bufferGeo.getOffsetInDataArray(1, 0)]);
        REQUIRE(pointers[2] == &data[bufferGeo.getOffsetInDataArray(2, 0)]);
    }
    
    SECTION("3D") {
        constexpr int N = 3;
        BufferGeometry<N> bufferGeo(2, 4, 2);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0, 0) == 0);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0, 1) == 1);
        CHECK(bufferGeo.getOffsetInDataArray(0, 1, 0) == 2);
        CHECK(bufferGeo.getOffsetInDataArray(0, 1, 1) == 3);
        CHECK(bufferGeo.getOffsetInDataArray(0, 2, 0) == 4);
        CHECK(bufferGeo.getOffsetInDataArray(0, 2, 1) == 5);
        CHECK(bufferGeo.getOffsetInDataArray(0, 3, 0) == 6);
        CHECK(bufferGeo.getOffsetInDataArray(0, 3, 1) == 7);
        CHECK(bufferGeo.getOffsetInDataArray(1, 0, 0) == 8);
        CHECK(bufferGeo.getOffsetInDataArray(1, 0, 1) == 9);
        CHECK(bufferGeo.getOffsetInDataArray(1, 1, 0) == 10);
        CHECK(bufferGeo.getOffsetInDataArray(1, 1, 1) == 11);
        CHECK(bufferGeo.getOffsetInDataArray(1, 2, 0) == 12);
        CHECK(bufferGeo.getOffsetInDataArray(1, 2, 1) == 13);
        CHECK(bufferGeo.getOffsetInDataArray(1, 3, 0) == 14);
        CHECK(bufferGeo.getOffsetInDataArray(1, 3, 1) == 15);
        
        CHECK(bufferGeo.getOffsetInPointerArray<0>(0) == 0);
        CHECK(bufferGeo.getOffsetInPointerArray<0>(1) == 1);

        CHECK(bufferGeo.getOffsetInPointerArray<1>(0, 0) == 2);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(0, 1) == 3);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(0, 2) == 4);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(0, 3) == 5);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(1, 0) == 6);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(1, 1) == 7);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(1, 2) == 8);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(1, 3) == 9);
        
        float data [2*4*2] {0};
        constexpr int pointerDimensions = N-1;
        float* pointers [VarArgOperations::sumOfCumulativeProductCapped(pointerDimensions, 2, 4, 2)];
        REQUIRE(getRawArrayLength(pointers) == 10);
        //bufferGeo.hookupPointerArrayToData(data, pointers);
//        REQUIRE(pointers[0] == &data[(bufferGeo.getOffsetInDataArray(0, 0, 0))]);
//        REQUIRE(pointers[1] == &data[(bufferGeo.getOffsetInDataArray(0, 1, 0))]);
//        REQUIRE(pointers[2] == &data[(bufferGeo.getOffsetInDataArray(0, 2, 0))]);
//        REQUIRE(pointers[3] == &data[(bufferGeo.getOffsetInDataArray(0, 3, 0))]);
//        REQUIRE(pointers[4] == &data[(bufferGeo.getOffsetInDataArray(1, 0, 0))]);
//        REQUIRE(pointers[5] == &data[(bufferGeo.getOffsetInDataArray(1, 1, 0))]);
//        REQUIRE(pointers[6] == &data[(bufferGeo.getOffsetInDataArray(1, 2, 0))]);
//        REQUIRE(pointers[7] == &data[(bufferGeo.getOffsetInDataArray(1, 3, 0))]);

    }
    SECTION("4D") {
        BufferGeometry<4> bufferGeo(1, 3, 2, 2);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0, 0, 0) == 0);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0, 0, 1) == 1);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0, 1, 0) == 2);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0, 1, 1) == 3);
        CHECK(bufferGeo.getOffsetInDataArray(0, 1, 0, 0) == 4);
        CHECK(bufferGeo.getOffsetInDataArray(0, 1, 0, 1) == 5);
        CHECK(bufferGeo.getOffsetInDataArray(0, 1, 1, 0) == 6);
        CHECK(bufferGeo.getOffsetInDataArray(0, 1, 1, 1) == 7);
        CHECK(bufferGeo.getOffsetInDataArray(0, 2, 0, 0) == 8);
        CHECK(bufferGeo.getOffsetInDataArray(0, 2, 0, 1) == 9);
        CHECK(bufferGeo.getOffsetInDataArray(0, 2, 1, 0) == 10);
        CHECK(bufferGeo.getOffsetInDataArray(0, 2, 1, 1) == 11);

        CHECK(bufferGeo.getOffsetInPointerArray<0>(0) == 0);

        CHECK(bufferGeo.getOffsetInPointerArray<1>(0, 0) == 1);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(0, 1) == 2);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(0, 2) == 3);

        CHECK(bufferGeo.getOffsetInPointerArray<2>(0, 0, 0) == 4);
        CHECK(bufferGeo.getOffsetInPointerArray<2>(0, 0, 1) == 5);
        CHECK(bufferGeo.getOffsetInPointerArray<2>(0, 1, 0) == 6);
        CHECK(bufferGeo.getOffsetInPointerArray<2>(0, 1, 1) == 7);
        CHECK(bufferGeo.getOffsetInPointerArray<2>(0, 2, 0) == 8);
        CHECK(bufferGeo.getOffsetInPointerArray<2>(0, 2, 1) == 9);
    }

}
