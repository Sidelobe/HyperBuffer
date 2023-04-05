//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2023 Lorenz Bucher - all rights reserved
//  https://github.com/Sidelobe/HyperBuffer

#include "TestCommon.hpp"

#include <vector>

#include "MemorySentinel.hpp"
#ifdef SLB_AMALGATED_HEADER
    #include "HyperBuffer.hpp"
#else
    #include "BufferGeometry.hpp"
#endif

using namespace slb;

TEST_CASE("BufferGeometry Tests")
{
    SECTION("1D") {
        constexpr int N = 1;
        BufferGeometry<N> bufferGeo(3);
        CHECK(bufferGeo.getDimensionExtents() == std::array<int, 1>{3});
        CHECK(bufferGeo.getDimensionExtentsPointer() != nullptr);
        CHECK(bufferGeo.getDimensionExtentsPointer()[0] == 3);
        
        float data [3] {0};
        float* pointers [1] {nullptr};
        bufferGeo.hookupPointerArrayToData(data, pointers);
        REQUIRE(pointers[0] == data);
        
        REQUIRE(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0) == 0);
        REQUIRE(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(1) == 1);
        REQUIRE(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(2) == 2);
    }
    
    SECTION("2D") {
        constexpr int N = 2;
        BufferGeometry<N> bufferGeo(3, 3);
        CHECK(bufferGeo.getDimensionExtents() == std::array<int, 2>{3, 3});
        CHECK(bufferGeo.getDimensionExtentsPointer() != nullptr);
        CHECK(bufferGeo.getDimensionExtentsPointer()[0] == 3);
        CHECK(bufferGeo.getDimensionExtentsPointer()[1] == 3);
        CHECK(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0) == 0);
        CHECK(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(1) == 3);
        CHECK(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(2) == 6);

        float data [3*3] {0};
        constexpr int pointerDimensions = N-1;
        float* pointers [CompiletimeMath::sumOfCumulativeProductCapped(pointerDimensions, 3, 3)] {nullptr};
        REQUIRE(getRawArrayLength(pointers) == 3);
        bufferGeo.hookupPointerArrayToData(data, pointers);
        REQUIRE(pointers[0] == &data[bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0)]);
        REQUIRE(pointers[1] == &data[bufferGeo.getDataArrayOffsetForHighestOrderSubDim(1)]);
        REQUIRE(pointers[2] == &data[bufferGeo.getDataArrayOffsetForHighestOrderSubDim(2)]);
    }

    SECTION("3D") {
        constexpr int N = 3;
        BufferGeometry<N> bufferGeo(2, 4, 2);
        CHECK(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0) == 0);
        CHECK(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(1) == 8);

        float data [CompiletimeMath::product(2, 4, 2)] {0};
        constexpr int pointerDimensions = N-1;
        float* pointers [CompiletimeMath::sumOfCumulativeProductCapped(pointerDimensions, 2, 4, 2)] {nullptr};
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
        
        // Verify pointers to data (lowest-order) dimension
        const int dataOffsetSubdim0 = bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0);
        REQUIRE(pointers3D[0][0] == &data[dataOffsetSubdim0 + 0]);
        REQUIRE(pointers3D[0][1] == &data[dataOffsetSubdim0 + 2]);
        REQUIRE(pointers3D[0][2] == &data[dataOffsetSubdim0 + 4]);
        REQUIRE(pointers3D[0][3] == &data[dataOffsetSubdim0 + 6]);

        const int dataOffsetSubdim1 = bufferGeo.getDataArrayOffsetForHighestOrderSubDim(1);
        REQUIRE(pointers3D[1][0] == &data[dataOffsetSubdim1 + 0]);
        REQUIRE(pointers3D[1][1] == &data[dataOffsetSubdim1 + 2]);
        REQUIRE(pointers3D[1][2] == &data[dataOffsetSubdim1 + 4]);
        REQUIRE(pointers3D[1][3] == &data[dataOffsetSubdim1 + 6]);
    }
    SECTION("4D") {
        constexpr int N = 4;
        BufferGeometry<N> bufferGeo(1, 3, 2, 2);
        REQUIRE(bufferGeo.getRequiredDataArraySize() == 12);
        CHECK(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0) == 0);

        float data [CompiletimeMath::product(1, 3, 2, 2)] {0};
        constexpr int pointerDimensions = N-1;
        float* pointers [CompiletimeMath::sumOfCumulativeProductCapped(pointerDimensions, 1, 3, 2, 2)] {nullptr};
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
        
        // Verify pointers to data (lowest-order) dimension
        const int dataOffsetSubdim0 = bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0);
        REQUIRE(pointers4D[0][0][0] == &data[dataOffsetSubdim0 + 0]);
        REQUIRE(pointers4D[0][0][1] == &data[dataOffsetSubdim0 + 2]);
        REQUIRE(pointers4D[0][1][0] == &data[dataOffsetSubdim0 + 4]);
        REQUIRE(pointers4D[0][1][1] == &data[dataOffsetSubdim0 + 6]);
        REQUIRE(pointers4D[0][2][0] == &data[dataOffsetSubdim0 + 8]);
        REQUIRE(pointers4D[0][2][1] == &data[dataOffsetSubdim0 + 10]);
    }
    SECTION("5D - sparse testing") {
        constexpr int N = 5;
        BufferGeometry<N> bufferGeo(2, 3, 2, 3, 6);
        CHECK(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0) == 0);
        CHECK(bufferGeo.getDataArrayOffsetForHighestOrderSubDim(1) == 108);

        float data [CompiletimeMath::product(2, 3, 2, 3, 6)] {0};
        constexpr int pointerDimensions = N-1;
        float* pointers [CompiletimeMath::sumOfCumulativeProductCapped(pointerDimensions, 2, 3, 2, 3, 6)] {nullptr};
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

        // Verify pointers to data (lowest-order) dimension   (sparse test)
        const int dataOffsetSubdim0 = bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0);
        REQUIRE(pointers5D[0][0][0][0] == &data[(dataOffsetSubdim0 + 0)]);
        REQUIRE(pointers5D[0][0][0][1] == &data[(dataOffsetSubdim0 + 6)]);
        REQUIRE(pointers5D[0][0][1][0] == &data[(dataOffsetSubdim0 + 3*2*3 +0)]); //+18
        REQUIRE(pointers5D[0][0][1][1] == &data[(dataOffsetSubdim0 + 3*2*3 +6)]); //+24
        REQUIRE(pointers5D[0][2][0][0] == &data[(dataOffsetSubdim0 + 2*2*3*2*3 +0)]); //+72
        
        const int dataOffsetSubdim1 = bufferGeo.getDataArrayOffsetForHighestOrderSubDim(1);
        REQUIRE(pointers5D[1][2][0][1] == &data[(dataOffsetSubdim1 + 2*2*3*2*3 +6)]);
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
    
    SECTION("Empty & singleton dimensions") {
        BufferGeometry<3> bufferGeo(1, 1, 1);
        REQUIRE(bufferGeo.getRequiredDataArraySize() == 1);
        REQUIRE(bufferGeo.getRequiredPointerArraySize() == 2);

        BufferGeometry<4>bufferGeo2(1, 1, 1, 0);
        REQUIRE(bufferGeo2.getRequiredDataArraySize() == 0);
        REQUIRE(bufferGeo2.getRequiredPointerArraySize() == 3);

        BufferGeometry<1> bufferGeo3(1);
        REQUIRE(bufferGeo3.getRequiredDataArraySize() == 1);
        REQUIRE(bufferGeo3.getRequiredPointerArraySize() == 1);
        
        BufferGeometry<1> bufferGeo4(0);
        REQUIRE(bufferGeo4.getRequiredDataArraySize() == 0);
        REQUIRE(bufferGeo4.getRequiredPointerArraySize() == 1);

        BufferGeometry<2> bufferGeo5(0, 0);
        REQUIRE(bufferGeo5.getRequiredDataArraySize() == 0);
        REQUIRE(bufferGeo5.getRequiredPointerArraySize() == 1);
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
        
        // Verify pointers to data (lowest-order) dimension
        float*** pointers3D = reinterpret_cast<float***>(pointers.data());
        const int dataOffsetSubdim0 = bufferGeo.getDataArrayOffsetForHighestOrderSubDim(0);
        REQUIRE(pointers3D[0][0] == &data[dataOffsetSubdim0 + 0]);
        REQUIRE(pointers3D[0][1] == &data[dataOffsetSubdim0 + 6]);
        REQUIRE(pointers3D[0][4] == &data[dataOffsetSubdim0 + 24]);

        const int dataOffsetSubdim1 = bufferGeo.getDataArrayOffsetForHighestOrderSubDim(1);
        REQUIRE(pointers3D[1][0] == &data[dataOffsetSubdim1 + 0]);
        REQUIRE(pointers3D[1][1] == &data[dataOffsetSubdim1 + 6]);
        REQUIRE(pointers3D[1][4] == &data[dataOffsetSubdim1 + 24]);
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

            bufferGeo1.getDataArrayOffsetForHighestOrderSubDim(1);
            bufferGeo2.getDataArrayOffsetForHighestOrderSubDim(1);
            bufferGeo3.getDataArrayOffsetForHighestOrderSubDim(1);
            bufferGeo4.getDataArrayOffsetForHighestOrderSubDim(1);
            bufferGeo5.getDataArrayOffsetForHighestOrderSubDim(1);

            auto e = bufferGeo5.getDimensionExtents(); UNUSED(e);
           
            float data [CompiletimeMath::product(2, 3, 2, 3, 6)] {0};
            constexpr int pointerDimensions = 4;
            float* pointers [CompiletimeMath::sumOfCumulativeProductCapped(pointerDimensions, 2, 3, 2, 3, 6)] {nullptr};
            bufferGeo5.hookupPointerArrayToData(data, pointers);

            // dims as array rather than var arg
            int dim1 = 2;
            int dim2 = 5;
            int dim3 = 6;
            std::array<int, 3> dims { dim1, dim2, dim3 };
            BufferGeometry<3> bufferGeo(dims);
        }
    }

    SECTION("Verify move assignment operation does not allocate") {
        BufferGeometry<4> bufferGeo4(2, 3, 2, 3);
        {
            ScopedMemorySentinel sentinel;
            BufferGeometry<4> bufferGeo4MovedTo = std::move(bufferGeo4);
            UNUSED(bufferGeo4MovedTo);
        }
    }
    SECTION("Verify move assignment ctor does not allocate") {
        BufferGeometry<4> bufferGeo4(2, 3, 2, 3);
        {
            ScopedMemorySentinel sentinel;
            BufferGeometry<4> bufferGeo4MovedTo(bufferGeo4);
            UNUSED(bufferGeo4MovedTo);
        }
    }
}
