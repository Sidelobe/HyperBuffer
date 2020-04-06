
#include <catch2/catch.hpp>

#include <vector>
#include <random>

#include "HyperBuffer.hpp"

template<typename T = float>
static inline std::vector<T> createRandomVector(int length, int seed=0)
{
    std::vector<T> result(length);
    std::mt19937 engine(seed);
    std::uniform_real_distribution<> dist(-1, 1); //(inclusive, inclusive)
    for (auto& sample : result) {
        sample = dist(engine);
    }
    return result;
}


TEST_CASE("HyperBuffer Tests - Internal Memory Allocation")
{
    SECTION("Build 1D owning") {
        HyperBuffer<int, 1> bufferI1(4);
        REQUIRE(bufferI1.dims()[0] == 4);
        REQUIRE(bufferI1.dim(0) == 4);
        bufferI1[0] = 0;
        bufferI1[1] = -1;
        bufferI1[2] = -2;
        bufferI1[3] = -3;
        REQUIRE(bufferI1.data() != nullptr);
        int* rawData = bufferI1.data();
        REQUIRE(rawData[0] == 0);
        REQUIRE(rawData[1] == -1);
        REQUIRE(rawData[2] == -2);
        REQUIRE(rawData[3] == -3);
    }
    SECTION("Build 2D owning") {
        HyperBuffer<int, 2> bufferI1(4, 2);
        REQUIRE(bufferI1.dims()[0] == 4);
        REQUIRE(bufferI1.dims()[1] == 2);
        REQUIRE(bufferI1.dim(1) == 2);
        bufferI1[0][0] = 0;
        bufferI1[0][1] = -1;
        bufferI1[0][2] = -2;
        bufferI1[0][3] = -3;
        bufferI1[1][0] = -10;
        bufferI1[1][1] = -11;
        bufferI1[1][2] = -22;
        bufferI1[1][3] = -33;
        REQUIRE(bufferI1.data() != nullptr);
        int** rawData = bufferI1.data();
        REQUIRE(rawData[0][0] == 0);
        REQUIRE(rawData[0][1] == -1);
        REQUIRE(rawData[0][2] == -2);
        REQUIRE(rawData[0][3] == -3);
        REQUIRE(rawData[1][0] == -10);
        REQUIRE(rawData[1][1] == -11);
        REQUIRE(rawData[1][2] == -22);
        REQUIRE(rawData[1][3] == -33);
        
    }
    SECTION("Build 3D owning") {
        HyperBuffer<float, 3> bufferF3(3, 3, 8);
        std::vector<int> dims(bufferF3.dims(), bufferF3.dims() + 3);
        REQUIRE(dims == std::vector<int>{3, 3, 8});

        std::vector<float> dataDim2_1 = createRandomVector(8, 333);
        std::vector<float> dataDim2_2 = createRandomVector(8, 666);
        std::vector<float> dataDim2_3 = createRandomVector(8, 999);
        float* dataDim1[] = { dataDim2_1.data(), dataDim2_2.data(), dataDim2_3.data() };
        float** dataDim0[] = { dataDim1, dataDim1, dataDim1 };

    }
}

TEST_CASE("HyperBuffer Tests - External Memory Allocation")
{
    std::vector<int>preAllocData { 1, 2, 3, 4, 5 };

                                        // not casting to int leads to very weird errors
    HyperBuffer<int, 1> buffer(preAllocData.data(), preAllocData.size()); // data is not copied
    std::vector<int> dims(buffer.dims(), buffer.dims() + 1);
    REQUIRE(dims == std::vector<int>{(int)preAllocData.size()});
    //buffer(0) = 99;
//    REQUIRE(buffer[0] == 99);
//    REQUIRE(buffer.dims()[0] == 5);
}

//TEST_CASE("Initializer List Construction")
//{
//    MultiDimArray<float, 1>  buffer1  {{3, 4, 5}};
//    MultiDimArray<float, 2>  buffer2 {{3, 4, 5}, {3, 4, 5}};
//
//}
//
//TEST_CASE("Sub-Array Assignmemt")
//{
//    MultiDimArray<float, 1>  buffer1  {{3, 4, 5}};
//    MultiDimArray<float, 2>  buffer2 {{3, 4, 5}, {3, 4, 5}};
//
//
//    MultiDimArray<float, 1> buffer2sub1 = buffer2(0);
//}

