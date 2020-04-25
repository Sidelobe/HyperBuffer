//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <catch2/catch.hpp>

#include <vector>
#include <random>
#include <numeric>

#include "HyperBuffer.hpp"
#include "MemorySentinel.hpp"

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

// functions to test the integrity of the different variants throught the same API
template<typename T, int N> void testHyperBuffer1D_size4(HyperBufferBase<T, N>& buffer);
template<typename T, int N> void testHyperBuffer2D_sizes2_4(HyperBufferBase<T, N>& buffer);
template<typename T, int N> void testHyperBuffer3D_sizes3_3_8(HyperBufferBase<T, N>& buffer);


TEST_CASE("HyperBuffer Tests - Internal Memory Allocation")
{
    SECTION("Build 1D owning") {
        constexpr int N = 1;
        HyperBuffer<int, N> buffer(4);
        testHyperBuffer1D_size4(buffer);
    }
    SECTION("Build 2D owning") {
        constexpr int N = 2;
        HyperBuffer<int, N> buffer(2, 4);
        testHyperBuffer2D_sizes2_4(buffer);
    }
    SECTION("Build 3D owning") {
        constexpr int N = 3;
        HyperBuffer<int, N> buffer(3, 3, 8);
        testHyperBuffer3D_sizes3_3_8(buffer);

        // Constructor via std::array
        std::array<int, N> dims  {3, 3, 8};
        HyperBuffer<int, N> bufferFromStdArray(dims);
        testHyperBuffer3D_sizes3_3_8(bufferFromStdArray);
    }
}

TEST_CASE("HyperBuffer Tests - External Memory Allocation (Flat)")
{
    SECTION("Build 1D owning") {
        int preAllocData[4];
        constexpr int N = 1;
        HyperBufferPreAllocFlat<int, N> buffer(preAllocData, 4);
        testHyperBuffer1D_size4(buffer);
    }
    SECTION("Build 2D owning") {
        constexpr int N = 2;
        int preAllocData[2*4];
        HyperBufferPreAllocFlat<int, N> buffer(preAllocData, 2, 4);
        testHyperBuffer2D_sizes2_4(buffer);
    }
    SECTION("Build 3D owning") {
        constexpr int N = 3;
        std::vector<int> preAllocData(3*3*8);
        std::iota(preAllocData.begin(), preAllocData.end(), 1);
        HyperBufferPreAllocFlat<int, N> buffer(preAllocData.data(), 3, 3, 8);
        
        REQUIRE(buffer[0][0][0] == 1);
        REQUIRE(buffer[0][0][7] == 8);
        REQUIRE(buffer[0][1][0] == 9);
        REQUIRE(buffer[1][0][0] == 25);
        REQUIRE(buffer[2][2][7] == 72);

        testHyperBuffer3D_sizes3_3_8(buffer);
        
        // Constructor via std::array
        std::array<int, N> dims  {3, 3, 8};
        HyperBufferPreAllocFlat<int, N> bufferFromStdArray(preAllocData.data(), dims);
        testHyperBuffer3D_sizes3_3_8(bufferFromStdArray);
        
    }
}

TEST_CASE("HyperBuffer Tests - External Memory Allocation (MultiDim)")
{
    // TODO
    std::vector<float> dataDim2_1 = createRandomVector(8, 333);
    std::vector<float> dataDim2_2 = createRandomVector(8, 666);
    std::vector<float> dataDim2_3 = createRandomVector(8, 999);
    float* dataDim1[] = { dataDim2_1.data(), dataDim2_2.data(), dataDim2_3.data() };
    float** dataDim0[] = { dataDim1, dataDim1, dataDim1 };
    
    
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


template<typename T, int N>
void testHyperBuffer1D_size4(HyperBufferBase<T, N>& buffer)
{
    REQUIRE(buffer.dim(0) == 4);
    REQUIRE(buffer.dims()[0] == 4);
    buffer[0] = 0;
    buffer[1] = -1;
    buffer[2] = -2;
    buffer[3] = -3;
    REQUIRE(buffer.data() != nullptr);
    int* rawData = buffer.data();
    REQUIRE(rawData[0] == 0);
    REQUIRE(rawData[1] == -1);
    REQUIRE(rawData[2] == -2);
    REQUIRE(rawData[3] == -3);
}

template<typename T, int N>
void testHyperBuffer2D_sizes2_4(HyperBufferBase<T, N>& buffer)
{
    REQUIRE(buffer.dims()[0] == 2);
    REQUIRE(buffer.dims()[1] == 4);
    REQUIRE(buffer.dim(1) == 4);
    buffer[0][0] = 0;
    buffer[0][1] = -1;
    buffer[0][2] = -2;
    buffer[0][3] = -3;
    buffer[1][0] = -10;
    buffer[1][1] = -11;
    buffer[1][2] = -22;
    buffer[1][3] = -33;
    REQUIRE(buffer.data() != nullptr);
    int** rawData = buffer.data();
    REQUIRE(rawData[0][0] == 0);
    REQUIRE(rawData[0][1] == -1);
    REQUIRE(rawData[0][2] == -2);
    REQUIRE(rawData[0][3] == -3);
    REQUIRE(rawData[1][0] == -10);
    REQUIRE(rawData[1][1] == -11);
    REQUIRE(rawData[1][2] == -22);
    REQUIRE(rawData[1][3] == -33);
}

template<typename T, int N> void
testHyperBuffer3D_sizes3_3_8(HyperBufferBase<T, N>& buffer)
{
    std::vector<int> dims(buffer.dims(), buffer.dims() + 3);
    REQUIRE(dims == std::vector<int>{3, 3, 8});    
    buffer[0][1][0] = -1;
    buffer[0][2][0] = -2;
    buffer[0][3][0] = -3;
    buffer[1][0][6] = -10;
    buffer[1][1][6] = -11;
    buffer[1][2][6] = -22;
    buffer[1][3][6] = -33;
    REQUIRE(buffer.data() != nullptr);
    int*** rawData = buffer.data();
    REQUIRE(rawData[0][1][0] == -1);
    REQUIRE(rawData[0][2][0] == -2);
    REQUIRE(rawData[0][3][0] == -3);
    REQUIRE(rawData[1][0][6] == -10);
    REQUIRE(rawData[1][1][6] == -11);
    REQUIRE(rawData[1][2][6] == -22);
    REQUIRE(rawData[1][3][6] == -33);
}
