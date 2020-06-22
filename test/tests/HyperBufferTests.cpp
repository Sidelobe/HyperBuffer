//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include "TestCommon.hpp"

#include <vector>
#include <random>
#include <numeric>

#include "HyperBuffer.hpp"
#include "MemorySentinel.hpp"

// functions to test the integrity of the different variants throught the same API
template<typename T> void testHyperBuffer1D_size4(HyperBufferBase<T, 1>& buffer);
template<typename T> void testHyperBuffer2D_sizes2_4(HyperBufferBase<T, 2>& buffer);
template<typename T> void testHyperBuffer3D_sizes3_3_8(HyperBufferBase<T, 3>& buffer);

// classic preprocessor hack to stringify -- double expansion is required
// https://gcc.gnu.org/onlinedocs/gcc-4.8.5/cpp/Stringification.html
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Helper to allocate 3D data on stack
#define BUILD_MULTIDIM_ON_STACK_3_3_8(VARNAME) \
    std::vector<int> dataDim0_0 = TestCommon::createRandomVectorInt(8, 333); \
    std::vector<int> dataDim0_1 = TestCommon::createRandomVectorInt(8, 666); \
    std::vector<int> dataDim0_2 = TestCommon::createRandomVectorInt(8, 999); \
    std::vector<int> dataDim1_0 = TestCommon::createRandomVectorInt(8, 1333); \
    std::vector<int> dataDim1_1 = TestCommon::createRandomVectorInt(8, 1666); \
    std::vector<int> dataDim1_2 = TestCommon::createRandomVectorInt(8, 1999); \
    std::vector<int> dataDim2_0 = TestCommon::createRandomVectorInt(8, 2333); \
    std::vector<int> dataDim2_1 = TestCommon::createRandomVectorInt(8, 2666); \
    std::vector<int> dataDim2_2 = TestCommon::createRandomVectorInt(8, 2999); \
    int* pointerDim1_0[] = { dataDim0_0.data(), dataDim0_1.data(), dataDim0_2.data() }; \
    int* pointerDim1_1[] = { dataDim1_0.data(), dataDim1_1.data(), dataDim1_2.data() }; \
    int* pointerDim1_2[] = { dataDim2_0.data(), dataDim2_1.data(), dataDim2_2.data() }; \
    int** VARNAME[] = { pointerDim1_0, pointerDim1_1, pointerDim1_2 };

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
    SECTION("Build 1D prealloc flat") {
        int preAllocData[4] {0};
        constexpr int N = 1;
        HyperBufferPreAllocFlat<int, N> buffer(preAllocData, 4);
        testHyperBuffer1D_size4(buffer);
    }
    SECTION("Build 2D prealloc flat") {
        constexpr int N = 2;
        int preAllocData[2*4] {0};
        HyperBufferPreAllocFlat<int, N> buffer(preAllocData, 2, 4);
        testHyperBuffer2D_sizes2_4(buffer);
    }
    SECTION("Build 3D prealloc flat") {
        constexpr int N = 3;
        std::vector<int> preAllocData(3*3*8, 0);
        std::iota(preAllocData.begin(), preAllocData.end(), 1);
        HyperBufferPreAllocFlat<int, N> buffer(preAllocData.data(), 3, 3, 8);
        
        REQUIRE(buffer[0][0][0] == 1);
        REQUIRE(buffer[0][0][7] == 8);
        REQUIRE(buffer[0][1][0] == 9);
        REQUIRE(buffer[1][0][0] == 25);
        REQUIRE(buffer[2][2][7] == 72);
        
        testHyperBuffer3D_sizes3_3_8(buffer);
        
        // Constructor via std::array
        std::array<int, N> dims {3, 3, 8};
        HyperBufferPreAllocFlat<int, N> bufferFromStdArray(preAllocData.data(), dims);
        testHyperBuffer3D_sizes3_3_8(bufferFromStdArray);
    }
}

TEST_CASE("HyperBuffer Tests - External Memory Allocation (MultiDim)")
{
    SECTION("Build 1D prealloc multidim") {
        constexpr int N = 1;
        std::vector<int> oneD = TestCommon::createRandomVectorInt(4, 123);
        HyperBufferPreAlloc<int, N> buffer(oneD.data(), oneD.size());
        testHyperBuffer1D_size4(buffer);

        std::array<int, 1> dims {4};
        HyperBufferPreAlloc<int, N> bufferFromStdArray(oneD.data(), dims); // dims as array
        testHyperBuffer1D_size4(bufferFromStdArray);
    }
    SECTION("Build 2D prealloc multidim") {
        constexpr int N = 2;
        std::vector<int> dataDim0_0 = TestCommon::createRandomVectorInt(4, 333);
        std::vector<int> dataDim0_1 = TestCommon::createRandomVectorInt(4, 666);
        int* dataDim1_0[] = { dataDim0_0.data(), dataDim0_1.data() };
        HyperBufferPreAlloc<int, N> buffer(dataDim1_0, 2, 4);
        testHyperBuffer2D_sizes2_4(buffer);
    }
    SECTION("Build 3D prealloc multidim") {
        constexpr int N = 3;
        BUILD_MULTIDIM_ON_STACK_3_3_8(multiDimData);
        HyperBufferPreAlloc<int, N> buffer(multiDimData, 3, 3, 8);
        testHyperBuffer3D_sizes3_3_8(buffer);

        // Constructor via std::array
        std::array<int, N> dims  {3, 3, 8};
        HyperBufferPreAlloc<int, N> bufferFromStdArray(multiDimData, dims);
        testHyperBuffer3D_sizes3_3_8(bufferFromStdArray);
    }

}

TEST_CASE("HyperBuffer ctor: different dimension variants")
{
    int dim1 = 3;
    int dim0 = 5;
    
    SECTION("owning") {
        HyperBuffer<int, 2> hostClass(dim1, dim0); // calls int ctor
        REQUIRE(hostClass.dims() == std::array<int, 2>({3, 5}));
        std::array<int, 2> dimArray = {3, 5};
        HyperBuffer<int, 2> hostClass2(dimArray); // calls array ctor
        REQUIRE(hostClass2.dims() == std::array<int, 2>({3, 5}));
        std::vector<int> dimVector = {3, 5};
        HyperBuffer<int, 2> hostClass3(dimVector); // calls int* ctor
        REQUIRE(hostClass3.dims() == std::array<int, 2>({3, 5}));
    }
    
    SECTION("prealloc flat") {
        int data [32];
        HyperBufferPreAllocFlat<int, 2> hostClass(data, dim1, dim0); // calls int ctor
        REQUIRE(hostClass.dims() == std::array<int, 2>({3, 5}));
        std::array<int, 2> dimArray = {3, 5};
        HyperBufferPreAllocFlat<int, 2> hostClass2(data, dimArray); // calls array ctor
        REQUIRE(hostClass2.dims() == std::array<int, 2>({3, 5}));
        std::vector<int> dimVector = {3, 5};
        HyperBufferPreAllocFlat<int, 2> hostClass3(data, dimVector); // calls int* ctor
        REQUIRE(hostClass3.dims() == std::array<int, 2>({3, 5}));
    }
    
    SECTION("prealloc") {
        int* data [32];
        HyperBufferPreAlloc<int, 2> hostClass(data, dim1, dim0); // calls int ctor
        REQUIRE(hostClass.dims() == std::array<int, 2>({3, 5}));
        std::array<int, 2> dimArray = {3, 5};
        HyperBufferPreAlloc<int, 2> hostClass2(data, dimArray); // calls array ctor
        REQUIRE(hostClass2.dims() == std::array<int, 2>({3, 5}));
        std::vector<int> dimVector = {3, 5};
        HyperBufferPreAlloc<int, 2> hostClass3(data, dimVector); // calls int* ctor
        REQUIRE(hostClass3.dims() == std::array<int, 2>({3, 5}));
    }
}

TEST_CASE("HyperBuffer: sub-buffer access")
{
    constexpr int N = 3;
    SECTION("owning") {
        HyperBuffer<int, N> buffer(2, 3, 8);
        int i = 0;
        for (int k=0; k < buffer.dim(0); ++k) {
            for (int l=0; l < buffer.dim(1); ++l) {
                for (int m=0; m < buffer.dim(2); ++m) {
                    buffer[k][l][m] = i++;
                }
            }
        }
        int subBufferIndex = GENERATE(0, 1);
        
        SECTION("ctor") {
            HyperBuffer<int, N-1> subBuffer(buffer, subBufferIndex);
            REQUIRE(subBuffer.dims() == std::array<int, N-1>{3, 8});
            const int start = 3*8 * subBufferIndex;
            int j = 0;
            for (int l=0; l < subBuffer.dim(0); ++l) {
                for (int m=0; m < subBuffer.dim(1); ++m) {
                    REQUIRE(subBuffer[l][m] == start + j++);
                }
            }
        }
//        SECTION("operator()") {
//            HyperBuffer<int, N-1> subBuffer = buffer(subBufferIndex);
//            REQUIRE(subBuffer.dims() == std::array<int, N-1>{3, 8});
//            const int start = 3*8 * subBufferIndex;
//            int j = 0;
//            for (int l=0; l < subBuffer.dim(0); ++l) {
//                for (int m=0; m < subBuffer.dim(1); ++m) {
//                    REQUIRE(subBuffer[l][m] == start + j++);
//                }
//            }
//        }
        
    }
    SECTION("prealloc flat") {
        int dataRaw1 [2*3*8];
        HyperBufferPreAllocFlat<int, N> buffer(dataRaw1, 2, 3, 8);
        int i = 0;
        for (int k=0; k < buffer.dim(0); ++k) {
            for (int l=0; l < buffer.dim(1); ++l) {
                for (int m=0; m < buffer.dim(2); ++m) {
                    buffer[k][l][m] = i++;
                }
            }
        }
        int subBufferIndex = GENERATE(0, 1);
        
        SECTION("ctor") {
            HyperBufferPreAllocFlat<int, N-1> subBuffer(buffer, subBufferIndex);
            REQUIRE(subBuffer.dims() == std::array<int, N-1>{3, 8});
            const int start = 3*8 * subBufferIndex;
            int j = 0;
            for (int l=0; l < subBuffer.dim(0); ++l) {
                for (int m=0; m < subBuffer.dim(1); ++m) {
                    REQUIRE(subBuffer[l][m] == start + j++);
                }
            }
        }
        SECTION("operator()") {
            HyperBufferPreAllocFlat<int, N-1> subBuffer = buffer(subBufferIndex);
            REQUIRE(subBuffer.dims() == std::array<int, N-1>{3, 8});
            const int start = 3*8 * subBufferIndex;
            int j = 0;
            for (int l=0; l < subBuffer.dim(0); ++l) {
                for (int m=0; m < subBuffer.dim(1); ++m) {
                    REQUIRE(subBuffer[l][m] == start + j++);
                }
            }
        }
        
        // TODO: Partial Assignment
//        int dataRaw2[8];
//        HyperBufferPreAllocFlat<int, 1> lowestDimData(dataRaw2, 8);
//        for (int k=0; k < lowestDimData.dim(0); ++k) {
//            lowestDimData[k] = -k;
//        }
//
//        buffer(0, 2) = lowestDimData;
//
//        for (int m=0; m < buffer.dim(2); ++m) {
//            REQUIRE(buffer[0][2][m] == -m);
//        }
    }
    SECTION("prealloc") {
        BUILD_MULTIDIM_ON_STACK_3_3_8(multiDimData);
        HyperBufferPreAlloc<int, N> buffer(multiDimData, 3, 3, 8);
        int i = 0;
        for (int k=0; k < buffer.dim(0); ++k) {
            for (int l=0; l < buffer.dim(1); ++l) {
                for (int m=0; m < buffer.dim(2); ++m) {
                    buffer[k][l][m] = i++;
                }
            }
        }
        int subBufferIndex = GENERATE(0, 1);

        SECTION("ctor") {
            HyperBufferPreAlloc<int, N-1> subBuffer(buffer, subBufferIndex);
            REQUIRE(subBuffer.dims() == std::array<int, N-1>{3, 8});
            const int start = 3*8 * subBufferIndex;
            int j = 0;
            for (int l=0; l < subBuffer.dim(0); ++l) {
                for (int m=0; m < subBuffer.dim(1); ++m) {
                    REQUIRE(subBuffer[l][m] == start + j++);
                }
            }
        }
     }
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

// MARK: - Data Verification

template<typename T>
void testHyperBuffer1D_size4(HyperBufferBase<T, 1>& buffer)
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
    
    rawData[1] = -99;
    REQUIRE(rawData[1] == -99);
    
    { // Verify all access operations do not allocate memory
        ScopedMemorySentinel sentinel;
        buffer[2] = -2;
        int d0 = buffer.dim(0);
        const int* dims = buffer.dims().data();
        auto dimsArray = buffer.dims();
        int* raw = buffer.data();
        UNUSED(d0);
        UNUSED(dims);
        UNUSED(dimsArray);
        UNUSED(raw);
    }
}

template<typename T>
void testHyperBuffer2D_sizes2_4(HyperBufferBase<T, 2>& buffer)
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
    
    { // Verify all access operations do not allocate memory
        ScopedMemorySentinel sentinel;
        buffer[0][2] = -2;
        int d0 = buffer.dim(0);
        const int* dims = buffer.dims().data();
        auto dimsArray = buffer.dims();
        int** raw = buffer.data();
        UNUSED(d0);
        UNUSED(dims);
        UNUSED(dimsArray);
        UNUSED(raw);
    }
}

template<typename T>
void testHyperBuffer3D_sizes3_3_8(HyperBufferBase<T, 3>& buffer)
{
    std::vector<int> dims(buffer.dims().begin(), buffer.dims().end());
    REQUIRE(dims == std::vector<int>{3, 3, 8});    
    buffer[0][1][0] = -1;
    buffer[0][2][0] = -2;
    buffer[1][0][6] = -10;
    buffer[1][1][6] = -11;
    buffer[1][2][6] = -22;
    buffer[2][2][6] = -33;
    REQUIRE(buffer.data() != nullptr);
    int*** rawData = buffer.data();
    REQUIRE(rawData[0][1][0] == -1);
    REQUIRE(rawData[0][2][0] == -2);
    REQUIRE(rawData[1][0][6] == -10);
    REQUIRE(rawData[1][1][6] == -11);
    REQUIRE(rawData[1][2][6] == -22);
    REQUIRE(rawData[2][2][6] == -33);
    
    { // Verify all access operations do not allocate memory
        ScopedMemorySentinel sentinel;
        buffer[0][2][0] = -2;
        int d0 = buffer.dim(0);
        const int* dimsPtr = buffer.dims().data();
        auto dimsArray = buffer.dims();
        int*** raw = buffer.data();
        UNUSED(d0);
        UNUSED(dimsPtr);
        UNUSED(dimsArray);
        UNUSED(raw);
    }
}
