
#include <catch2/catch.hpp>

#include "HyperBuffer.hpp"

TEST_CASE("HyperBuffer Tests")
{
    SECTION("Build 1D owning") {
        HyperBuffer<int, 1> arrayI1(4);
        REQUIRE(arrayI1.dims()[0] == 4);
        arrayI1[0] = 0;
        arrayI1[1] = -1;
        arrayI1[2] = -2;
        arrayI1[3] = -3;
        REQUIRE(arrayI1.data() != nullptr);
        int* rawData = arrayI1.data();
        REQUIRE(rawData[0] == 0);
        REQUIRE(rawData[1] == -1);
        REQUIRE(rawData[2] == -2);
        REQUIRE(rawData[3] == -3);
    }
    SECTION("Build 2D owning") {
        HyperBuffer<int, 2> arrayI1(4, 2);
        REQUIRE(arrayI1.dims()[0] == 4);
        REQUIRE(arrayI1.dims()[1] == 2);
        arrayI1[0][0] = 0;
        arrayI1[0][1] = -1;
        arrayI1[0][2] = -2;
        arrayI1[0][3] = -3;
        arrayI1[1][0] = -10;
        arrayI1[1][1] = -11;
        arrayI1[1][2] = -22;
        arrayI1[1][3] = -33;
        REQUIRE(arrayI1.data() != nullptr);
        int** rawData = arrayI1.data();
        REQUIRE(rawData[0][0] == 0);
        REQUIRE(rawData[0][1] == -1);
        REQUIRE(rawData[0][2] == -2);
        REQUIRE(rawData[0][3] == -3);
        REQUIRE(rawData[1][0] == -10);
        REQUIRE(rawData[1][1] == -11);
        REQUIRE(rawData[1][2] == -22);
        REQUIRE(rawData[1][3] == -33);
    }

    //HyperBuffer<float, 3> arrayF3(3, 3);

    
}

TEST_CASE("External allocation")
{
    int buffer[] { 1, 2, 3, 4, 5 };

    HyperBuffer<int, 1> array(&buffer[0], 5);   // data is not copied
    //array(0) = 99;
//    REQUIRE(buffer[0] == 99);
//    REQUIRE(array.dims()[0] == 5);
}

//TEST_CASE("Initializer List Construction")
//{
//    MultiDimArray<float, 1>  array1  {{3, 4, 5}};
//    MultiDimArray<float, 2>  array2 {{3, 4, 5}, {3, 4, 5}};
//
//}
//
//TEST_CASE("Sub-Array Assignmemt")
//{
//    MultiDimArray<float, 1>  array1  {{3, 4, 5}};
//    MultiDimArray<float, 2>  array2 {{3, 4, 5}, {3, 4, 5}};
//
//
//    MultiDimArray<float, 1> array2sub1 = array2(0);
//}

