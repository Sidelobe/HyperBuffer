//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <catch2/catch.hpp>

#include <vector>

#include "BufferGeometry.hpp"
#include "MemorySentinel.hpp"

TEST_CASE("BufferGeometry Tests")
{
    SECTION("1D") {
        constexpr int N = 1;
        BufferGeometry<N> bufferGeo(3);
        CHECK(bufferGeo.getOffsetInDataArray(0) == 0);
        CHECK(bufferGeo.getOffsetInDataArray(1) == 1);
        CHECK(bufferGeo.getOffsetInDataArray(2) == 2);
        
        float data [3] {0};
        float* pointers [1] {nullptr};
        bufferGeo.hookupPointerArrayToData(data, pointers);
        REQUIRE(pointers[0] == data);
        
        CHECK(bufferGeo.getOffsetInPointerArray<0>(2) == 2);
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
        float* pointers [VarArgOperations::sumOfCumulativeProductCapped(pointerDimensions, 3, 3)] {nullptr};
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
        
        float data [VarArgOperations::product(2, 4, 2)] {0};
        constexpr int pointerDimensions = N-1;
        float* pointers [VarArgOperations::sumOfCumulativeProductCapped(pointerDimensions, 2, 4, 2)] {nullptr};
        REQUIRE(getRawArrayLength(pointers) == 10);
        
        bufferGeo.hookupPointerArrayToData(data, pointers);
        
        for (int i=0; i<getRawArrayLength(pointers); ++i) {
            REQUIRE(pointers[i] != nullptr);
        }
        
        float** pointers2D = reinterpret_cast<float**>(pointers);
        float*** pointers3D = reinterpret_cast<float***>(pointers);
        REQUIRE(pointers2D[0] == pointers[0]);
        REQUIRE(pointers2D[1] == pointers[1]);
        REQUIRE(pointers3D[0][0] == pointers[2]);
        REQUIRE(pointers3D[0][1] == pointers[3]);
        REQUIRE(pointers3D[0][2] == pointers[4]);
        REQUIRE(pointers3D[0][3] == pointers[5]);
        REQUIRE(pointers3D[1][0] == pointers[6]);
        REQUIRE(pointers3D[1][1] == pointers[7]);
        REQUIRE(pointers3D[1][2] == pointers[8]);
        REQUIRE(pointers3D[1][3] == pointers[9]);

        REQUIRE((float**)(pointers[0]) == &(pointers[2]));
        REQUIRE((float**)(pointers[1]) == &(pointers[6]));
        REQUIRE(pointers[2] == &data[(bufferGeo.getOffsetInDataArray(0, 0, 0))]);
        REQUIRE(pointers[3] == &data[(bufferGeo.getOffsetInDataArray(0, 1, 0))]);
        REQUIRE(pointers[4] == &data[(bufferGeo.getOffsetInDataArray(0, 2, 0))]);
        REQUIRE(pointers[5] == &data[(bufferGeo.getOffsetInDataArray(0, 3, 0))]);
        REQUIRE(pointers[6] == &data[(bufferGeo.getOffsetInDataArray(1, 0, 0))]);
        REQUIRE(pointers[7] == &data[(bufferGeo.getOffsetInDataArray(1, 1, 0))]);
        REQUIRE(pointers[8] == &data[(bufferGeo.getOffsetInDataArray(1, 2, 0))]);
        REQUIRE(pointers[9] == &data[(bufferGeo.getOffsetInDataArray(1, 3, 0))]);

    }
    SECTION("4D") {
        constexpr int N = 4;
        BufferGeometry<N> bufferGeo(1, 3, 2, 2);

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
    
        float data [VarArgOperations::product(1, 3, 2, 2)] {0};
        constexpr int pointerDimensions = N-1;
        float* pointers [VarArgOperations::sumOfCumulativeProductCapped(pointerDimensions, 1, 3, 2, 2)] {nullptr};
        REQUIRE(getRawArrayLength(pointers) == 10);
        
        bufferGeo.hookupPointerArrayToData(data, pointers);
        
        for (int i=0; i<getRawArrayLength(pointers); ++i) {
            REQUIRE(pointers[i] != nullptr);
        }
        
        float** pointers2D = reinterpret_cast<float**>(pointers);
        float*** pointers3D = reinterpret_cast<float***>(pointers);
        float**** pointers4D = reinterpret_cast<float****>(pointers);
        REQUIRE(pointers2D[0] == pointers[0]);
        REQUIRE(pointers3D[0][0] == pointers[1]);
        REQUIRE(pointers3D[0][1] == pointers[2]);
        REQUIRE(pointers3D[0][2] == pointers[3]);
        REQUIRE(pointers4D[0][0][0] == pointers[4]);
        REQUIRE(pointers4D[0][0][1] == pointers[5]);
        REQUIRE(pointers4D[0][1][0] == pointers[6]);
        REQUIRE(pointers4D[0][1][1] == pointers[7]);
        REQUIRE(pointers4D[0][2][0] == pointers[8]);
        REQUIRE(pointers4D[0][2][1] == pointers[9]);

        REQUIRE((float**)(pointers[0]) == &(pointers[1]));
        REQUIRE((float**)(pointers[1]) == &(pointers[4]));
        REQUIRE((float**)(pointers[2]) == &(pointers[6]));
        REQUIRE((float**)(pointers[3]) == &(pointers[8]));
        REQUIRE(pointers[4] == &data[(bufferGeo.getOffsetInDataArray(0, 0, 0, 0))]);
        REQUIRE(pointers[5] == &data[(bufferGeo.getOffsetInDataArray(0, 0, 1, 0))]);
        REQUIRE(pointers[6] == &data[(bufferGeo.getOffsetInDataArray(0, 1, 0, 0))]);
        REQUIRE(pointers[7] == &data[(bufferGeo.getOffsetInDataArray(0, 1, 1, 0))]);
        REQUIRE(pointers[8] == &data[(bufferGeo.getOffsetInDataArray(0, 2, 0, 0))]);
        REQUIRE(pointers[9] == &data[(bufferGeo.getOffsetInDataArray(0, 2, 1, 0))]);
    }
    
    SECTION("5D - sparse testing") {
        constexpr int N = 5;
        BufferGeometry<N> bufferGeo(2, 3, 2, 3, 6);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0, 0, 1, 0) == 6);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0, 0, 2, 5) == 17);
        CHECK(bufferGeo.getOffsetInDataArray(0, 0, 1, 0, 0) == 18);
        CHECK(bufferGeo.getOffsetInDataArray(1, 0, 0, 0, 0) == 108);
        CHECK(bufferGeo.getOffsetInDataArray(1, 2, 1, 2, 5) == 215);

        CHECK(bufferGeo.getOffsetInPointerArray<0>(0) == 0);
        CHECK(bufferGeo.getOffsetInPointerArray<1>(0, 0) == 2);
        CHECK(bufferGeo.getOffsetInPointerArray<2>(0, 0, 0) == 8);
        CHECK(bufferGeo.getOffsetInPointerArray<2>(0, 1, 0) == 14);
        CHECK(bufferGeo.getOffsetInPointerArray<2>(1, 0, 0) == 26);
        CHECK(bufferGeo.getOffsetInPointerArray<3>(1, 0, 0, 0) == 38);
        CHECK(bufferGeo.getOffsetInPointerArray<3>(1, 1, 1, 1) == 48);
    
        float data [VarArgOperations::product(2, 3, 2, 3, 6)] {0};
        constexpr int pointerDimensions = N-1;
        float* pointers [VarArgOperations::sumOfCumulativeProductCapped(pointerDimensions, 2, 3, 2, 3, 6)] {nullptr};
        REQUIRE(getRawArrayLength(pointers) == 56);
        REQUIRE(bufferGeo.getRequiredDataArraySize() == getRawArrayLength(data));
        REQUIRE(bufferGeo.getRequiredPointerArraySize() == getRawArrayLength(pointers));

        bufferGeo.hookupPointerArrayToData(data, pointers);
        
        for (int i=0; i<getRawArrayLength(pointers); ++i) {
            REQUIRE(pointers[i] != nullptr);
        }
        
        float** pointers2D = reinterpret_cast<float**>(pointers);
        float*** pointers3D = reinterpret_cast<float***>(pointers);
        float**** pointers4D = reinterpret_cast<float****>(pointers);
        float***** pointers5D = reinterpret_cast<float*****>(pointers);
        REQUIRE(pointers2D[0] == pointers[0]);
        REQUIRE(pointers2D[1] == pointers[1]);
        REQUIRE(pointers3D[0][0] == pointers[2]);
        REQUIRE(pointers3D[0][2] == pointers[4]);
        REQUIRE(pointers3D[1][2] == pointers[7]);
        REQUIRE(pointers4D[0][0][0] == pointers[8]);
        REQUIRE(pointers4D[0][2][0] == pointers[12]);
        REQUIRE(pointers4D[0][2][1] == pointers[13]);
        REQUIRE(pointers4D[1][0][0] == pointers[14]);
        REQUIRE(pointers4D[1][1][1] == pointers[17]);
        REQUIRE(pointers5D[0][0][0][0] == pointers[20]);
        REQUIRE(pointers5D[1][0][0][0] == pointers[38]);


        REQUIRE(pointers5D[0][0][0][0] == &data[(bufferGeo.getOffsetInDataArray(0, 0, 0, 0, 0))]);
        REQUIRE(pointers5D[0][0][0][1] == &data[(bufferGeo.getOffsetInDataArray(0, 0, 0, 1, 0))]);
        REQUIRE(pointers5D[0][0][1][0] == &data[(bufferGeo.getOffsetInDataArray(0, 0, 1, 0, 0))]);
        REQUIRE(pointers5D[0][0][1][1] == &data[(bufferGeo.getOffsetInDataArray(0, 0, 1, 1, 0))]);
        REQUIRE(pointers5D[0][2][0][0] == &data[(bufferGeo.getOffsetInDataArray(0, 2, 0, 0, 0))]);
        REQUIRE(pointers5D[1][2][0][1] == &data[(bufferGeo.getOffsetInDataArray(1, 2, 0, 1, 0))]);
    }
    
    SECTION("Absurdly high dimension") {
        constexpr int N = 32;
        BufferGeometry<N> bufferGeo(2, 2, 2, 2, 2, 2, 2, 2,
                                    2, 2, 2, 2, 2, 2, 2, 2,
                                    2, 2, 2, 2, 2, 2, 2, 2,
                                    2, 2, 2, 2, 2, 2, 2, 2);
        
        std::array<int, N> dims{3};
        BufferGeometry<N> bufferGeo2(dims);
    }
    
    SECTION("Allocation in Dynamic containers") {
        constexpr int N = 3;
        int dim1 = 2;
        int dim2 = 5;
        int dim3 = 6;
        
        std::array<int, N> dims;
        dims[0] = dim1;
        dims[1] = dim2;
        dims[2] = dim3;
        
        BufferGeometry<N> bufferGeo(dims);
        
        std::vector<float> data(bufferGeo.getRequiredDataArraySize(), 0);
        std::vector<float*> pointers(bufferGeo.getRequiredPointerArraySize(), nullptr);
        REQUIRE(data.size() == 60);
        REQUIRE(pointers.size() == 12);

        bufferGeo.hookupPointerArrayToData(data.data(), pointers.data());
        
        for (int i=0; i < pointers.size(); ++i) {
            REQUIRE(pointers[i] != nullptr);
        }

        REQUIRE((float**)(pointers[0]) == &(pointers[2]));
        REQUIRE((float**)(pointers[1]) == &(pointers[7]));
        REQUIRE(pointers[2] == &data[(bufferGeo.getOffsetInDataArray(0, 0, 0))]);
        REQUIRE(pointers[6] == &data[(bufferGeo.getOffsetInDataArray(0, 4, 0))]);
        REQUIRE(pointers[7] == &data[(bufferGeo.getOffsetInDataArray(1, 0, 0))]);
        REQUIRE(pointers[11] == &data[(bufferGeo.getOffsetInDataArray(1, 4, 0))]);
    }
    
    SECTION("Dynamic Memory Allocation Tests") {
        // Verify none of these operations allocate memory dynamically
        {
            ScopedMemorySentinel sentinel;
            
            BufferGeometry<1> bufferGeo1(2);
            BufferGeometry<2> bufferGeo2(2, 3);
            BufferGeometry<3> bufferGeo3(2, 3, 2);
            BufferGeometry<4> bufferGeo4(2, 3, 2, 3);
            BufferGeometry<5> bufferGeo5(2, 3, 2, 3, 6);
            
            bufferGeo1.getOffsetInDataArray(1);
            bufferGeo2.getOffsetInDataArray(0, 1);
            bufferGeo3.getOffsetInDataArray(1, 0, 1);
            bufferGeo4.getOffsetInDataArray(0, 1, 0, 1);
            bufferGeo5.getOffsetInDataArray(0, 0, 0, 1, 0);
            
            bufferGeo5.getOffsetInPointerArray<0>(0);
            bufferGeo5.getOffsetInPointerArray<1>(0, 0);
            bufferGeo5.getOffsetInPointerArray<2>(1, 0, 0);
            bufferGeo5.getOffsetInPointerArray<3>(1, 1, 1, 1);
            
            float data [VarArgOperations::product(2, 3, 2, 3, 6)] {0};
            constexpr int pointerDimensions = 4;
            float* pointers [VarArgOperations::sumOfCumulativeProductCapped(pointerDimensions, 2, 3, 2, 3, 6)] {nullptr};
            bufferGeo5.hookupPointerArrayToData(data, pointers);
            
            // dims as array rather than var arg
            int dim1 = 2;
            int dim2 = 5;
            int dim3 = 6;
            std::array<int, 3> dims { dim1, dim2, dim3 };
            BufferGeometry<3> bufferGeo(dims);
        }
    }
}