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

using namespace slb;

// functions to test the integrity of the different variants throught the same API
template<typename U> void testHyperBuffer1D_size4(HyperBufferBase<int, 1, U>& buffer);
template<typename U> void testHyperBuffer2D_sizes2_4(HyperBufferBase<int, 2, U>& buffer);
template<typename U> void testHyperBuffer3D_sizes3_3_8(HyperBufferBase<int, 3, U>& buffer);

// helper lambda
static auto fillWith3DSequence = [](auto& buffer)
{
    int i = 0;
    for (int k=0; k < buffer.dim(0); ++k) {
        for (int l=0; l < buffer.dim(1); ++l) {
            for (int m=0; m < buffer.dim(2); ++m) {
                buffer[k][l][m] = i++;
            }
        }
    }
};


TEST_CASE("HyperBuffer Tests - Construction and Data Access")
{
    // test operator() read & write (cannot do this through base class API)
    auto verify1D = [](auto& buffer)
    {
        testHyperBuffer1D_size4(buffer);
        buffer[2] = 666;
        REQUIRE(buffer.at(2) == 666);
        buffer.at(2) = -2;
        REQUIRE(buffer[2] == -2);
    };
    auto verify2D = [](auto& buffer)
    {
        testHyperBuffer2D_sizes2_4(buffer);
        buffer[1][2] = 666;
        REQUIRE(buffer.at(1, 2) == 666);
        buffer.at(1, 2) = -2;
        REQUIRE(buffer[1][2] == -2);
    };
    auto verify3D = [](auto& buffer)
    {
        testHyperBuffer3D_sizes3_3_8(buffer);
        buffer[1][2][6] = 666;
        REQUIRE(buffer.at(1, 2, 6) == 666);
        buffer.at(1, 2, 6) = -22;
        REQUIRE(buffer[1][2][6] == -22);
    };
    
    SECTION("Internal Memory Allocation") {
        HyperBuffer<int, 1> buffer1D(4);
        verify1D(buffer1D);
        
        HyperBuffer<int, 2> buffer2D(2, 4);
        verify2D(buffer2D);
        
        HyperBuffer<int, 3> buffer3D(3, 3, 8);
        verify3D(buffer3D);

        // Constructor via std::array
        std::array<int, 3> dims {3, 3, 8};
        HyperBuffer<int, 3> bufferFromStdArray(dims);
        verify3D(bufferFromStdArray);
    }
    
    SECTION("External Memory Allocation (Flat)") {
        int preAllocData1D[4] {0};
        HyperBufferPreAllocFlat<int, 1> buffer1D(preAllocData1D, 4);
        verify1D(buffer1D);

        int preAllocData2D[2*4] {0};
        HyperBufferPreAllocFlat<int, 2> buffer2D(preAllocData2D, 2, 4);
        verify2D(buffer2D);

        std::vector<int> preAllocData3D(3*3*8, 0);
        HyperBufferPreAllocFlat<int, 3> buffer3D(preAllocData3D.data(), 3, 3, 8);
        verify3D(buffer3D);

        // Constructor via std::array
        std::array<int, 3> dims {3, 3, 8};
        HyperBufferPreAllocFlat<int, 3> bufferFromStdArray(preAllocData3D.data(), dims);
        verify3D(bufferFromStdArray);
    }

    SECTION("External Memory Allocation (MultiDim)") {
        std::vector<int> oneD = TestCommon::createRandomVectorInt(4, 123);
        HyperBufferPreAlloc<int, 1> buffer1D(oneD.data(), oneD.size());
        verify1D(buffer1D);

        std::vector<int> xdataDim0_0 = TestCommon::createRandomVectorInt(4, 333);
        std::vector<int> xdataDim0_1 = TestCommon::createRandomVectorInt(4, 666);
        int* xdataDim1_0[] = { xdataDim0_0.data(), xdataDim0_1.data() };
        HyperBufferPreAlloc<int, 2> buffer2D(xdataDim1_0, 2, 4);
        verify2D(buffer2D);

        BUILD_MULTIDIM_ON_STACK_3_3_8(multiDimData);
        HyperBufferPreAlloc<int, 3> buffer3D(multiDimData, 3, 3, 8);
        verify3D(buffer3D);

        // Constructor via std::array
        std::array<int, 3> dims  {3, 3, 8};
        HyperBufferPreAlloc<int, 3> bufferFromStdArray(multiDimData, dims);
        verify3D(bufferFromStdArray);
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

TEST_CASE("HyperBuffer const correctness")
{
    auto verify = [](auto& buffer)
    {
        // create a const accessor
        const auto& constBuffer = buffer;
        
        // verify data read access
        REQUIRE(buffer.at(0, 0, 7) == 7);
        REQUIRE(constBuffer.at(0, 0, 7) == 7);

        // static checks: verify we can assign to a const, but not to a non-const
        static_assert(std::is_assignable<const int*&, decltype(constBuffer.at(0, 1).data())>::value == true, "Can assign to a const");
        static_assert(std::is_assignable<int*&,       decltype(constBuffer.at(0, 1).data())>::value == false, "Cannot assign to a non-const");
        static_assert(std::is_assignable<const int* const*&, decltype(constBuffer.at(2).data())>::value == true, "Can assign to a const");
        static_assert(std::is_assignable<int**&,             decltype(constBuffer.at(2).data())>::value == false, "Cannot assign to a non-const");
        static_assert(std::is_assignable<const int*&,  decltype(constBuffer[0][0])>::value == true, "Can assign to a const");
        static_assert(std::is_assignable<int*&,        decltype(constBuffer[0][0])>::value == false, "Cannot assign to a non-const");
        static_assert(std::is_assignable<const int* const*&, decltype(constBuffer[0])>::value == true, "Can assign to a const");
        static_assert(std::is_assignable<int**&,             decltype(constBuffer[0])>::value == false, "Cannot assign to a non-const");
        static_assert(std::is_assignable<int* const*&,       decltype(constBuffer[0])>::value == false, "Cannot assign to a non-const");
        static_assert(std::is_assignable<const int**&,       decltype(constBuffer[0])>::value == false, "Cannot assign to a non-const");

        static_assert(std::is_assignable<const int* const* const*&, decltype(constBuffer.data())>::value == true, "Can assign to a const");
        static_assert(std::is_assignable<const int* const**&, decltype(constBuffer.data())>::value == false, "Cannot assign to a non-const");
        static_assert(std::is_assignable<const int** const*&, decltype(constBuffer.data())>::value == false, "Cannot assign to a non-const");
        static_assert(std::is_assignable<int* const* const*&, decltype(constBuffer.data())>::value == false, "Cannot assign to a non-const");
        static_assert(std::is_assignable<int***&,             decltype(constBuffer.data())>::value == false, "Cannot assign to a non-const");

        static_assert(std::is_trivially_assignable<decltype(constBuffer.at(0, 0, 7)), int>::value == false, "Cannot write to a const");
        static_assert(std::is_trivially_assignable<decltype(constBuffer[0][0][7]), int>::value == false, "Cannot write to a const");
        static_assert(std::is_trivially_assignable<decltype(constBuffer.data()[0][0][7]), int>::value == false, "Cannot write to a const");
        static_assert(std::is_trivially_assignable<decltype(constBuffer.at(0, 1).data()), int*>::value == false, "Cannot write to a const");
        static_assert(std::is_trivially_assignable<decltype(constBuffer.at(1).data()), int*>::value == false, "Cannot write to a const");
    };

    SECTION("owning") {
        HyperBuffer<int, 3> buffer(3, 3, 8);
        fillWith3DSequence(buffer);
        verify(buffer);
        
        // Explicitly test a 1D owning, because above verify function only checks HyperBufferPreAllocFlat after being reduced to subBuffers
        const HyperBuffer<int, 1> constBuffer1D(4);
        int a = constBuffer1D.at(2); (UNUSED(a));
        static_assert(std::is_trivially_assignable<int&, decltype(constBuffer1D.at(1))>::value == true, "Can assign to a const");
        static_assert(std::is_trivially_assignable<decltype(constBuffer1D.at(1)), int*>::value == false, "Cannot write to a const");
    }
    SECTION("prealloc flat") {
        int dataRaw1 [3*3*8];
        HyperBufferPreAllocFlat<int, 3> buffer(dataRaw1, 3, 3, 8);
        fillWith3DSequence(buffer);
        verify(buffer);
    }
    SECTION("prealloc") {
        BUILD_MULTIDIM_ON_STACK_3_3_8(multiDimData);
        HyperBufferPreAlloc<int, 3> buffer(multiDimData, 3, 3, 8);
        fillWith3DSequence(buffer);
        verify(buffer);
    }
}

TEST_CASE("HyperBuffer: sub-buffer construction & operator() access")
{
    // assumes a {3, 3, 8} buffer
    auto verify = [](auto& buffer)
    {
        // RW Access to data via operator()
        REQUIRE(buffer.at(0, 1, 5) == 13);
        buffer.at(0, 1, 5) = -13;
        REQUIRE(buffer.at(0, 1, 5) == -13);
        buffer.at(0, 1, 5) = 13; // restore original value
        
        // N-1 Sub-buffer -> 2D
        int subBufferIndex = GENERATE(0, 1, 2);
        
        auto subBuffer = buffer.at(subBufferIndex);
        REQUIRE(subBuffer.dims() == std::array<int, 2>{buffer.dim(1), buffer.dim(2)});
        int j = 0;
        for (int l=0; l < subBuffer.dim(0); ++l) {
            for (int m=0; m < subBuffer.dim(1); ++m) {
                REQUIRE(subBuffer[l][m] == buffer.dim(1) * buffer.dim(2) * subBufferIndex + j++);
            }
        }
        
        // N-2 Sub-buffer -> 1D
        int subBufferIndex2 = 1; // just test one value
        auto subBuffer2 = buffer.at(subBufferIndex, subBufferIndex2);
        REQUIRE(subBuffer2.dims() == std::array<int, 1>{buffer.dim(2)});
        j = 0;
        for (int m=0; m < subBuffer2.dim(0); ++m) {
            REQUIRE(subBuffer2[m] == buffer.dim(1) * buffer.dim(2) * subBufferIndex + buffer.dim(2) * subBufferIndex2 + j++);
        }
    };
    
    SECTION("owning") {
        HyperBuffer<int, 3> buffer(3, 3, 8);
        fillWith3DSequence(buffer);
        
        // NOTE: A sub-buffer of HyperBuffer is a HyperBufferPreAllocFlat pointing to the HyperBuffer's data!
        
        verify(buffer);
    }
    
    SECTION("prealloc flat") {
        int dataRaw1 [3*3*8];
        HyperBufferPreAllocFlat<int, 3> buffer(dataRaw1, 3, 3, 8);
        fillWith3DSequence(buffer);
        verify(buffer);
    }
    SECTION("prealloc") {
        BUILD_MULTIDIM_ON_STACK_3_3_8(multiDimData);
        HyperBufferPreAlloc<int, 3> buffer(multiDimData, 3, 3, 8);
        fillWith3DSequence(buffer);
        verify(buffer);
        
        { // Creating a subbuffer also does not allocate memory
            ScopedMemorySentinel sentinel;
            auto subBuffer = buffer.at(1);
        }
    }
}

TEST_CASE("HyperBuffer: Sub-Buffer Assignmemt")
{
    HyperBuffer<int, 3> buffer(2, 2, 4);
    fillWith3DSequence(buffer);
           
    HyperBuffer<int, 1> bufferData(4);
    bufferData[0] = -1;
    bufferData[1] = -2;
    bufferData[2] = -3;
    bufferData[3] = -4;
    
    //TODO: Sub-Buffer assigment
    //buffer.at(0,0) = bufferData;
}

// MARK: - Data Verification

template<typename U>
void testHyperBuffer1D_size4(HyperBufferBase<int, 1, U>& buffer)
{
    REQUIRE(buffer.dims() == std::array<int, 1>{4});
    REQUIRE(buffer.dim(0) == 4);
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
        int d0 = buffer.dim(0); UNUSED(d0);
        const int* dims = buffer.dims().data(); UNUSED(dims);
        auto dimsArray = buffer.dims(); UNUSED(dimsArray);
        int* raw = buffer.data(); UNUSED(raw);
        const auto& constBuffer = buffer;
        const int* rawConst = constBuffer.data(); UNUSED(rawConst);
    }
}

template<typename U>
void testHyperBuffer2D_sizes2_4(HyperBufferBase<int, 2, U>& buffer)
{
    REQUIRE(buffer.dims() == std::array<int, 2>{2, 4});
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
        int d0 = buffer.dim(0); UNUSED(d0);
        const int* dims = buffer.dims().data(); UNUSED(dims);
        auto dimsArray = buffer.dims(); UNUSED(dimsArray);
        int** raw = buffer.data(); UNUSED(raw);
    }
}

template<typename U>
void testHyperBuffer3D_sizes3_3_8(HyperBufferBase<int, 3, U>& buffer)
{
    REQUIRE(buffer.dims() == std::array<int, 3>{3, 3, 8});
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
        int d0 = buffer.dim(0); UNUSED(d0);
        const int* dimsPtr = buffer.dims().data(); UNUSED(dimsPtr);
        auto dimsArray = buffer.dims(); UNUSED(dimsArray);
        int*** raw = buffer.data();     UNUSED(raw);
        const auto& constBuffer = buffer;
        const int* const* const* rawConst = constBuffer.data(); UNUSED(rawConst);
    }
}
