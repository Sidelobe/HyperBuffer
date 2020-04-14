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
        BufferGeometry<1> bufferGeo(3);
        CHECK(bufferGeo.getOffsetInDataArray(0) == 0);
        CHECK(bufferGeo.getOffsetInDataArray(1) == 1);
        CHECK(bufferGeo.getOffsetInDataArray(2) == 2);
       
//        float data [3] {0};
//        float* pointers [3];
//        bufferGeo.hookupPointerArrayToData(data, pointers);
        
//        CHECK(bufferGeo.getOffsetInPointerArray(0, 0) == 0);
//        CHECK(bufferGeo.getOffsetInPointerArray(1, 0) == 1);
//        CHECK(bufferGeo.getOffsetInPointerArray(2, 0) == 2);
//        CHECK(pointers[0] == &data[(bufferGeo.getOffsetForIndex(0))]);
//        CHECK(pointers[1] == &data[(bufferGeo.getOffsetForIndex(1))]);
//        CHECK(pointers[2] == &data[(bufferGeo.getOffsetForIndex(2))]);
    }
    
    SECTION("2D") {
        BufferGeometry<2> bufferGeo(3, 3);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0) == 0);
        CHECK(bufferGeo.getOffsetInDataArray(0, 1) == 1);
        CHECK(bufferGeo.getOffsetInDataArray(0, 2) == 2);
        CHECK(bufferGeo.getOffsetInDataArray(1, 0) == 3);
        CHECK(bufferGeo.getOffsetInDataArray(1, 1) == 4);
        CHECK(bufferGeo.getOffsetInDataArray(1, 2) == 5);
        CHECK(bufferGeo.getOffsetInDataArray(2, 0) == 6);
        CHECK(bufferGeo.getOffsetInDataArray(2, 1) == 7);
        CHECK(bufferGeo.getOffsetInDataArray(2, 2) == 8);

//        CHECK(bufferGeo.getOffsetInPointerArray(0, 0) == 0);
//        CHECK(bufferGeo.getOffsetInPointerArray(1, 0) == 1);
//        CHECK(bufferGeo.getOffsetInPointerArray(2, 0) == 2);

    
//        float data [3*3] {0};
//        float* pointers [3];
//        bufferGeo.hookupPointerArrayToData(data, pointers);
//        REQUIRE(pointers[0] == &data[(bufferGeo.getOffsetForIndex(0, 0))]);
//        REQUIRE(pointers[1] == &data[(bufferGeo.getOffsetForIndex(1, 0))]);
//        REQUIRE(pointers[2] == &data[(bufferGeo.getOffsetForIndex(2, 0))]);
    }
    
    SECTION("3D") {
        BufferGeometry<3> bufferGeo(2, 4, 2);
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
