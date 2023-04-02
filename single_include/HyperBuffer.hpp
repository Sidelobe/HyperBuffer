// NOTE: This file is an amalgamation of individual source files to create a single-header include

//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2021 Lorenz Bucher - all rights reserved

#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <memory>
#include <sstream>
#include <type_traits>
#include <vector>

extern "C" 
{
}

// MARK: -------- TemplateUtils.hpp --------
//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved



#define UNUSED(x) (void)x

/* Macro to detect if exceptions are disabled (works on GCC, Clang and MSVC) 3 */
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#if !__has_feature(cxx_exceptions) && !defined(__cpp_exceptions) && !defined(__EXCEPTIONS) && !defined(_CPPUNWIND)
  #define EXCEPTIONS_DISABLED
#endif

/* these functions are coming in C++17, MSVC already defines them */
#if (__cplusplus < 201703) && !defined(_MSC_VER)
namespace std
{
// MARK: - std::as_const
template <class T>
static constexpr std::add_const_t<T>& as_const(T& t) noexcept { return t; }
} // namespace std
#endif

namespace slb
{

// MARK: - Assertion handling
namespace Assertions
{
#ifndef ASSERT
    #define ASSERT(condition, ...) Assertions::handleAssert(#condition, condition, __FILE__, __LINE__, ##__VA_ARGS__)
#endif
#ifndef ASSERT_ALWAYS
    #define ASSERT_ALWAYS(...) Assertions::handleAssert("", false, __FILE__, __LINE__, ##__VA_ARGS__)
#endif

/**
 * Custom assertion handler
 *
 * @note: this assertion handler is constexpr - to allow its use inside constexpr functions.
 * The handler will still be evaluated at runtime, but memory is only allocated IF the assertion is triggered.
 */
static constexpr void handleAssert(const char* conditionAsText, bool condition, const char* file, int line, const char* message = "")
{
    if (condition == true) {
        return;
    }
    
#ifdef EXCEPTIONS_DISABLED
    UNUSED(conditionAsText); UNUSED(file); UNUSED(line); UNUSED(message);
    assert(0 && message);
#else
    throw std::runtime_error(std::string("Assertion failed: ") + conditionAsText + " (" + file + ":" + std::to_string(line) + ") " + message);
#endif
}
} // namespace Assertions

// MARK: - Add pointers to type
// Recursive Template trick to add an arbitrary number of pointers to a type
template<class T, int N>
struct add_pointers_to_type
{
    using type = typename add_pointers_to_type<T*, N-1>::type;
};

template<class T>
struct add_pointers_to_type<T, 0>
{
    using type = T;
};

static_assert(std::is_same<add_pointers_to_type<int,1>::type, int*>{}, "");
static_assert(std::is_same<add_pointers_to_type<float,3>::type, float***>{}, "");
static_assert(std::is_same<add_pointers_to_type<float,0>::type, float>{}, "");

// MARK: - Add const pointers to type
// Recursive Template trick to add an arbitrary number of const pointers to a type
template<class T, int N>
struct add_const_pointers_to_type
{
    using type = typename add_const_pointers_to_type<const T*, N-1>::type;
};

template<class T>
struct add_const_pointers_to_type<T, 0>
{
    using type = T;
};

static_assert(std::is_same<add_const_pointers_to_type<int,1>::type, int const*>{}, "");
static_assert(std::is_same<add_const_pointers_to_type<int,2>::type, int const* const*>{}, "");
static_assert(std::is_same<add_const_pointers_to_type<float,3>::type, float const* const* const*>{}, "");
static_assert(std::is_same<add_const_pointers_to_type<float,0>::type, float>{}, "");

// MARK: - Remove pointers from type
// Recursive Template trick to remove an arbitrary number of pointers from a type

template<class T, int N>
struct remove_pointers_from_type
{
    using type = typename remove_pointers_from_type<typename std::remove_pointer<T>::type, N-1>::type;
};

template<class T>
struct remove_pointers_from_type<T, 0>
{
    using type = T;
};

static_assert(std::is_same<remove_pointers_from_type<int*,1>::type, int>{}, "");
static_assert(std::is_same<remove_pointers_from_type<float***,2>::type, float*>{}, "");
static_assert(std::is_same<remove_pointers_from_type<float**,2>::type, float>{}, "");
static_assert(std::is_same<remove_pointers_from_type<float,0>::type, float>{}, "");

// Recursive Template trick to remove all pointers from a type
template<class T>
struct remove_all_pointers_from_type
{
    using type = T;
};

template<class T>
struct remove_all_pointers_from_type<T*>
{
    using type = typename remove_all_pointers_from_type<T>::type;
};

static_assert(std::is_same<remove_all_pointers_from_type<float***>::type, float>{}, "");
static_assert(std::is_same<remove_all_pointers_from_type<int**>::type, int>{}, "");
static_assert(std::is_same<remove_all_pointers_from_type<float>::type, float>{}, "");

/**
 * Returns the size of a static C array in number of elements. Also works for multidimensional arrays.
 */
template<class T> constexpr int getRawArrayLength(const T& a)
{
    return sizeof(a) / sizeof(typename std::remove_all_extents<T>::type);
}


namespace VarArgOperations
{

// MARK: - Tuple creation
// Make Tuple from std::array
template<std::size_t... I, std::size_t N>
constexpr auto makeIntTuple(const std::array<int, N>& arr, std::index_sequence<I...>) noexcept
{
    return std::make_tuple(arr[I]...);
}

template<std::size_t N>
constexpr auto makeIntTuple(const std::array<int, N>& arr) noexcept
{
    return makeIntTuple(arr, std::make_index_sequence<N>{});
}

// Make Tuple from Raw Array
template<std::size_t... I, std::size_t N>
constexpr auto makeIntTuple(const int (&arr)[N], std::index_sequence<I...>) noexcept
{
    return std::make_tuple(arr[I]...);
}

template<std::size_t N>
constexpr auto makeIntTuple(const int (&arr)[N]) noexcept
{
    return makeIntTuple(arr, std::make_index_sequence<N>{});
}


// MARK: std::apply - like implementation
/**
 * Helper to call a function with a std::tuple of arguments (standardized in C++17 as std::apply)
 * source: https://essential-cpp.programming-books.io/iterating-with-stdinteger-sequence-cca589107b7a499e9e7275427a994f97
 */
namespace detail
{
    template <typename F, class Tuple, std::size_t... Is>
    constexpr auto apply_impl(F&& f, Tuple&& tpl, std::index_sequence<Is...> ) noexcept
    {
        return std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(tpl))...);
    }
}

// apply from std::tuple
template <typename F, class Tuple>
constexpr auto apply(F&& f, Tuple&& tpl) noexcept
{
    return detail::apply_impl(std::forward<F>(f),
                              std::forward<Tuple>(tpl),
                              std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

// apply from std::array (directly)
template <typename F, std::size_t N>
constexpr auto apply(F&& f, const std::array<int, N>& arr) noexcept
{
    return detail::apply_impl(std::forward<F>(f), makeIntTuple(arr), std::make_index_sequence<N>{});
}

// apply from int array (directly)
template <typename F, std::size_t N>
constexpr auto apply(F&& f, const int (&array)[N]) noexcept
{
    return detail::apply_impl(std::forward<F>(f), makeIntTuple(array), std::make_index_sequence<N>{});
}


} // namespace VarArgOperations


} // namespace slb

// MARK: -------- CompiletimeMath.hpp --------
//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved



namespace slb
{

// MARK: - Variadic arguments / parameter pack helpers -- [resolved at compile time!]
namespace CompiletimeMath
{

// MARK: - areAllPositive
/** @returns true if every element of the parameter pack is > 0 [base case] */
template<typename T>
constexpr bool areAllPositive(T first) noexcept { return first > T{0}; }

/** @returns true if every element of the parameter pack is > 0 [recursive] */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr bool areAllPositive(T first, Args... args) noexcept
{
    return areAllPositive(first) && areAllPositive(args...);
}

// MARK: - Sum
/** Calculate the sum of a given number of args in parameter pack - starting from the given firstSummand (1-based) */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumOverRange(int firstSummand, int numSummands, Args... args) noexcept
{
    firstSummand = std::max<int>(firstSummand, 1);
    numSummands = std::min<int>(numSummands, static_cast<int>(sizeof...(args)) - firstSummand + 1);
    
    T sum {0};
    T values[] { args... };
    for (int i=firstSummand; i < firstSummand+numSummands; ++i) {
        sum += values[i-1];
    }
    return sum;
}

/** Calculate the sum of a given number of args in parameter pack (starting with the first one) */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumCapped(int numSummands, Args... args) noexcept
{
    return sumOverRange(1, numSummands, args...);
}

/** Calculate the sum of all args in parameter pack */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sum(Args... args) noexcept
{
    return sumCapped(sizeof...(args), args...);
}

// MARK: - Product

/** Calculate the product of a given number of args in parameter pack - starting from the given firstFactor (1-based) */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T productOverRange(int firstFactor, int numFactors, Args... args) noexcept
{
    T product{1};
    if (numFactors <= 0) { product = {0}; }
    firstFactor = std::max<int>(firstFactor, 1);
    numFactors = std::min<int>(numFactors, static_cast<int>(sizeof...(args)) - firstFactor + 1);
    
    T values[]{ args... };
     for (int i=firstFactor; i < firstFactor+numFactors; ++i) {
        product *= values[i-1];
    }
    return product;
}

/** Calculate the product of a given number of args in parameter pack - starting with the first one */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T productCapped(int numSummands, Args... args) noexcept
{
    return productOverRange(1, numSummands, args...);
}

/** Multiply all args in parameter pack */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T product(Args... args) noexcept
{
    return productCapped(sizeof...(args), args...);
}

// MARK: - Sum of Cumulative Product

/**
 * Calculate the sum of cumulative products of a given number of args in parameter pack
 * -- starting from the given firstElement (1-based)
 */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumOfCumulativeProductOverRange(int firstElement, int numElements, Args... args) noexcept
{
    firstElement = std::max<int>(firstElement, 1);
    numElements = std::min<int>(numElements, static_cast<int>(sizeof...(args)) - firstElement + 1);
    
    T sum{0};
    T values[]{ args... };
    for (int i=firstElement; i < firstElement+numElements; ++i) {
        T cumulativeProduct{1};
        for (int j=0; j <= i-1; ++j) {
            cumulativeProduct *= values[j];
        }
        sum += cumulativeProduct;
    }
    return sum;
}

/** Calculate the sum of cumulative products of a given number of args in parameter pack -- starting from the first one */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumOfCumulativeProductCapped(int cap, Args... args) noexcept
{
    return sumOfCumulativeProductOverRange(1, cap, args...);
}

/**
 * Calculate the sum of cumulative products of all args in parameter pack.
 *  @note: This is equivalent to this Matlab/octave command: sum(cumprod(...))
 */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumOfCumulativeProduct(Args... args) noexcept
{
    return sumOfCumulativeProductCapped(sizeof...(Args), args...);
}

} // namespace CompiletimeMath
} // namespace slb

// MARK: -------- IntArrayOperations.hpp --------
//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved




// ---------------------------------------------------------------------------------------------------------------------
// These are runtime function-wrappers designed to allow the use of the compile-time utility functions in VarArgOperations::
//
// NOTE: I'm sure there's a more elegant way of doing this, but so far I've not managed.
// ---------------------------------------------------------------------------------------------------------------------

namespace slb
{

// MARK: - std::array
namespace StdArrayOperations
{

/** @see CompiletimeMath::product */
template<std::size_t N>
constexpr int product(const std::array<int, N>& array) noexcept
{
    return VarArgOperations::apply([](auto&&... args)
    {
        return CompiletimeMath::product(std::forward<decltype(args)>(args)...);
    }, array);
}

/** @see CompiletimeMath::productCapped */
template<std::size_t N>
constexpr int productCapped(int cap, const std::array<int, N>& array) noexcept
{
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return CompiletimeMath::productCapped(cap, std::forward<decltype(args)>(args)...);
    }, array);
}

/** @see CompiletimeMath::sumOfCumulativeProductCapped */
template<std::size_t N>
constexpr int sumOfCumulativeProductCapped(int cap, const std::array<int, N>& array) noexcept
{
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return CompiletimeMath::sumOfCumulativeProductCapped(cap, std::forward<decltype(args)>(args)...);
    }, array);
}

/** @returns the N-1 sub-array : removes / "shaves off" the first element */
template<std::size_t N>
constexpr std::array<int, N-1> shaveOffFirstElement(const std::array<int, N>& array) noexcept
{
    std::array<int, N-1> subarray;
    for (int i=0; i < N-1; ++i) {
        subarray[i] = array[i+1];
    }
    return subarray;
}


} // namespace StdArrayOperations
} // namespace slb

// MARK: -------- BufferGeometry.hpp --------
//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved




namespace slb
{

/**
 * This class performs the 'geometry' calculations for certain set of dimension extents.
 * It allows multi-dimensional access to one-dimensional memory.
 *
 * The underlying data model resembles two self-referencing flat (1D) arrays that contain all the pointers (for every
 * dimension except the lowest-order) and all the data (lowest-order dimension).
 */
template<int N>
class BufferGeometry
{
public:
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit BufferGeometry(I... i) noexcept : m_dimensionExtents{i...}
    {
        static_assert(sizeof...(I) == N, "Incorrect number of arguments");
    }
    
    /** Constructor that takes the extents of the dimensions as a std::array */
    explicit BufferGeometry(const std::array<int, N>& dimensionExtents) noexcept : m_dimensionExtents(dimensionExtents) {}
    
    /** Constructor that takes the extents of the dimensions as a std::vector */
    explicit BufferGeometry(const std::vector<int>& dimensionExtents)
    {
        ASSERT(dimensionExtents.size() == N, "Incorrect number of dimension extents");
        std::copy(dimensionExtents.begin(), dimensionExtents.end(), m_dimensionExtents.begin());
    }

    const std::array<int, N>& getDimensionExtents() const noexcept { return m_dimensionExtents; }
    const int* getDimensionExtentsPointer() const noexcept { return m_dimensionExtents.data(); }
    
    /** @return the number of required data entries (lowest-order dimension) given the configured geometry */
    int getRequiredDataArraySize() const noexcept
    {
        return StdArrayOperations::product(m_dimensionExtents);
    }
    
    /** @return the number of required pointer entries given the configured geometry */
    int getRequiredPointerArraySize() const noexcept
    {
        return std::max(StdArrayOperations::sumOfCumulativeProductCapped(N-1, m_dimensionExtents), 1); // at least size 1
    }
    
    /**
     * Calculates the offset of where data that belongs to a specific (given index) highest-order sub-dimension starts.
     * e.g. if the highest-order dimension's extent is 2, all data for index=0 is located in the first half of
     * the **data** array and the all data for index=1 in the second half.
     */
    int getDataArrayOffsetForHighestOrderSubDim(int index) const
    {
        const int totalNumDataEntries = getRequiredDataArraySize();
        ASSERT(index < m_dimensionExtents[0], "Index out of range");
        ASSERT(totalNumDataEntries % m_dimensionExtents[0] == 0, "Internal error in buffer geometry!");
        return index * totalNumDataEntries / m_dimensionExtents[0];
    }
    
    /**
     * Set up the supplied pointer array as a self-referencing array and point the lowest dimension
     * pointers at the supplied data array.
     *
     * @param dataArray an array of T. Size must match result given by getRequiredDataArraySize()
     * @param pointerArray an array of T*. Size must match result given by getRequiredPointerArraySize()
     */
    template<typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
    void hookupPointerArrayToData(T* dataArray, T** pointerArray) const
    {
        if (N == 1) {
            pointerArray[0] = dataArray;
            return;
        }
        
        // Intertwine pointer array: connect all pointers to itself -- skips 2 lowest-order dimensions
        int dataPointerStartOffset = hookupHigherDimPointers(pointerArray, 0, 0);
  
        // Get number of data pointers (length of 2nd-lowest dim)
        int numDataPointers = StdArrayOperations::productCapped(N-1, m_dimensionExtents);

        //Hook up pointer that point to data (second lowest-order dimension)
        for (int i=0; i < numDataPointers; ++i) {
            int offsetInDataArray = i * m_dimensionExtents[N-1];
            pointerArray[dataPointerStartOffset + i] = &dataArray[offsetInDataArray];
        }
    }
    
private:
    template<typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
    int hookupHigherDimPointers(T** pointerArray, int arrayIndex, int dimIndex) const noexcept
    {
        if (dimIndex >= N-2) {
            return arrayIndex; // end recursion
        }

        int startOfNextDimension = StdArrayOperations::sumOfCumulativeProductCapped(dimIndex+1, m_dimensionExtents);
        int numPointersInThisDimension = StdArrayOperations::productCapped(dimIndex+1, m_dimensionExtents);

        for (int index = 0; index < numPointersInThisDimension; ++index) {
            int nextDimExtent = m_dimensionExtents[dimIndex + 1];
            int offset = startOfNextDimension + nextDimExtent * index;
            // hook up pointer to element of next dimension
            pointerArray[arrayIndex + index] = reinterpret_cast<T*>(&(pointerArray)[offset]);
        }
        
        // recursive call to lower-order dimension
        arrayIndex += numPointersInThisDimension;
        return hookupHigherDimPointers(pointerArray, arrayIndex, dimIndex+1);
    }
                                                     
private:
    std::array<int, N> m_dimensionExtents;
    
};

} // namespace slb

// MARK: -------- HyperBufferStoragePolicies.hpp --------
//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2021 Lorenz Bucher - all rights reserved




namespace slb
{

template<typename T, int N> class StoragePolicyView; // forward declaration

/**
 *  Native memory model for HyperBuffer: full ownership of data and pointer memory.
 *  The extents of the dimensions have to be supplied during construction.
 *
 *  Memory for the pointers and the data is allocated separately, but each in a 1-dimensional block of memory, which
 *  results in only two allocations for the entire multi-dimensional data, regardless of the dimensions.
 *
 *  - Template parameters: T=data type (e.g. float),  N=dimension (e.g. 3)
 */
template<typename T, int N>
class StoragePolicyOwning
{
    using size_type                 = int;
    using pointer_type              = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type        = typename add_const_pointers_to_type<T, N>::type;

public:
    using SubBufferPolicy = StoragePolicyView<T, N-1>; // SubBuffers of an 'owning' are always a 'view' !
    
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    explicit StoragePolicyOwning(I... i) :
        m_bufferGeometry(i...),
        m_data(m_bufferGeometry.getRequiredDataArraySize()),
        m_pointers(m_bufferGeometry.getRequiredPointerArraySize())
    {
        ASSERT(CompiletimeMath::areAllPositive(i...), "Invalid Dimension extents");
        m_bufferGeometry.hookupPointerArrayToData(m_data.data(), m_pointers.data());
    }
    
    /** @return a modifiable pointer to a subdimension of the data */
    T* getSubDimData(size_type index) const
    {
        const int offset = m_bufferGeometry.getDataArrayOffsetForHighestOrderSubDim(index);
        return getRawData(offset);
    }

    int size(int i) const { ASSERT(i < N); return m_bufferGeometry.getDimensionExtents()[i]; }
    const std::array<int, N>& sizes() const noexcept { return m_bufferGeometry.getDimensionExtents(); }

    const_pointer_type getDataPointer_Nx() const noexcept { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
          pointer_type getDataPointer_Nx()       noexcept { return reinterpret_cast<pointer_type>(m_pointers.data()); }
              const T* getDataPointer_N1() const noexcept { return *m_pointers.data(); }
                    T* getDataPointer_N1()       noexcept { return *m_pointers.data(); }

private:
    /**
     * @returns a pointer to the raw data at a given offset.
     * @note The const_cast is unfortunately necessary to resolve an ambiguity in the scenario of creating a subBuffer
     * view from an owning buffer (this rabbithole is deep...)
     */
    T* getRawData(int offset = 0) const
    {
        return const_cast<T*>(&m_data[offset]);
    }

private:
    friend class StoragePolicyView<T, N>;
    
    /** Handles the geometry (organization) of the data memory, enabling multi-dimensional access to it */
    BufferGeometry<N> m_bufferGeometry;
    
    /** All the data (innermost dimension) is stored in a 1D structure and access with offsets to simulate multi-dimensionality */
    std::vector<T> m_data;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    std::vector<T*> m_pointers;
};


// ====================================================================================================================
/**
 *  A wrapper for existing HyperBuffer data in its native format, which gives it the same API, but without data ownership.
 *  The extents of the dimensions have to be supplied during construction.
 *
 *  The pre-allocated data is expected to be in a flat (one-dimensional), contiguous memory block. Pointer memory is
 *  allocated during construction and, unlike data memory, is owned by a given instance of this class.
 *
 *  - Template parameters: T=data type (e.g. float),  N=dimension (e.g. 3)
 */
template<typename T, int N>
class StoragePolicyView
{
    using size_type                 = int;
    using pointer_type              = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type        = typename add_const_pointers_to_type<T, N>::type;
    
public:
    using SubBufferPolicy = StoragePolicyView<T, N-1>;
    
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    StoragePolicyView(T* preAllocatedDataFlat, I... i) :
        m_bufferGeometry(i...),
        m_externalData(preAllocatedDataFlat),
        m_pointers(m_bufferGeometry.getRequiredPointerArraySize())
    {
        ASSERT(CompiletimeMath::areAllPositive(i...), "Invalid Dimension extents");
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }

    /** Constructor that takes an existing (owning) Buffer and creates a (non-owning) View from it */
    explicit StoragePolicyView(StoragePolicyOwning<T, N>& owningBufferPolicy) :
        m_bufferGeometry(owningBufferPolicy.sizes()),
        m_externalData(owningBufferPolicy.getRawData()),
        m_pointers(m_bufferGeometry.getRequiredPointerArraySize())
    {
        m_bufferGeometry.hookupPointerArrayToData(m_externalData, m_pointers.data());
    }
    
    /** @return a modifiable pointer to a subdimension of the data */
    T* getSubDimData(size_type index) const
    {
        const int offset = m_bufferGeometry.getDataArrayOffsetForHighestOrderSubDim(index);
        return &m_externalData[offset];
    }
    
    int size(int i) const { ASSERT(i < N); return m_bufferGeometry.getDimensionExtents()[i]; }
    const std::array<int, N>& sizes() const noexcept { return m_bufferGeometry.getDimensionExtents(); }

    const_pointer_type getDataPointer_Nx() const noexcept { return reinterpret_cast<const_pointer_type>(m_pointers.data()); }
          pointer_type getDataPointer_Nx()       noexcept { return reinterpret_cast<pointer_type>(m_pointers.data()); }
              const T* getDataPointer_N1() const noexcept { return *m_pointers.data(); }
                    T* getDataPointer_N1()       noexcept { return *m_pointers.data(); }
    
private:
    /** Handles the geometry (organization) of the data memory, enabling multi-dimensional access to it */
    BufferGeometry<N> m_bufferGeometry;
    
    /** Pointer to the externally-allocated data memory */
    T* m_externalData;
    
    /** All but the innermost dimensions consist of pointers only, which are stored in a 1D structure as well */
    std::vector<T*> m_pointers;
};

// ====================================================================================================================
/**
 *  A wrapper for existing multi-dimensional data (e.g. float**), giving it the same API as HyperBuffer. The extents
 *  of the dimensions have to be supplied during construction. Both pointer and data memory are stored externally
 *  (this class has no ownership).
 *
 *  The pre-allocated data is expected to be stored in individual, non-contiguous memory blocks. Only the lowest-orderd
 *
 *  - Template parameters: T=data type (e.g. float),  N=dimension (e.g. 3)
 *
 */
template<typename T, int N>
class StoragePolicyViewNonContiguous
{
    using size_type                 = int;
    using pointer_type              = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type        = typename add_const_pointers_to_type<T, N>::type;
    using subdim_pointer_type       = typename remove_pointers_from_type<pointer_type, 1>::type;
    
public:
    using SubBufferPolicy = StoragePolicyViewNonContiguous<T, N-1>;
    
    /** Constructor that takes the extents of the dimensions as a variable argument list */
    template<typename... I>
    StoragePolicyViewNonContiguous(pointer_type preAllocatedData, I... i) :
        m_dimensionExtents{static_cast<int>(i)...},
        m_externalData(preAllocatedData)
    {
        ASSERT(CompiletimeMath::areAllPositive(i...), "Invalid Dimension extents");
    }
            
    /** Constructor that takes the extents of the dimensions as a std::array */
    StoragePolicyViewNonContiguous(pointer_type preAllocatedData, std::array<int, N> dimensionExtents) :
        m_dimensionExtents(dimensionExtents),
        m_externalData(preAllocatedData)
    {
        ASSERT(CompiletimeMath::areAllPositive(m_dimensionExtents), "Invalid Dimension extents");
    }
    
    /** Constructor that takes the extents of the dimensions as a std::vector */
    StoragePolicyViewNonContiguous(pointer_type preAllocatedData, std::vector<int> dimensionExtentsVector) :
        m_externalData(preAllocatedData)
    {
        ASSERT(dimensionExtentsVector.size() == N, "Incorrect number of dimension extents");
        std::copy(dimensionExtentsVector.begin(), dimensionExtentsVector.end(), m_dimensionExtents.begin());
        ASSERT(CompiletimeMath::areAllPositive(m_dimensionExtents), "Invalid Dimension extents");
    }
    
    /** @return a modifiable pointer to a subdimension of the data */
    subdim_pointer_type getSubDimData(size_type index) const
    {
        return m_externalData[index];
    }

    int size(int i) const { ASSERT(i < N); return m_dimensionExtents[i]; }
    const std::array<int, N>& sizes() const noexcept { return m_dimensionExtents; }

    const_pointer_type getDataPointer_Nx() const noexcept { return m_externalData; }
          pointer_type getDataPointer_Nx()       noexcept { return m_externalData; }
              const T* getDataPointer_N1() const noexcept { return reinterpret_cast<const T*>(m_externalData); }
                    T* getDataPointer_N1()       noexcept { return reinterpret_cast<T*>(m_externalData); }
    
private:
    std::array<int, N> m_dimensionExtents;
    
    /** Pointer to the externally-allocated multi-dimensional data memory */
    pointer_type m_externalData;
};


} // namespace slb

// Macros to restrict a function declaration to certain use cases, e.g. 1-dimensional, higher-dimensional, ...
#define FOR_N1 template<int M=N, std::enable_if_t<(M==1), int> = 0>
#define FOR_Nx template<int M=N, std::enable_if_t<(M>1), int> = 0>

// For N>1 and any number / exactly N-1 variable arguments
#define FOR_Nx_V template<int M=N, typename... I, std::enable_if_t<(M>1 && sizeof...(I)<M-1), int> = 0>
#define FOR_Nx_N template<int M=N, typename... I, std::enable_if_t<(M>1 && sizeof...(I)==M-1), int> = 0>

namespace slb
{

/**
 *  HyperBuffer is a container for dynamically-allocated N-dimensional datasets. The extents of the dimensions have
 *  to be supplied during construction and are not changeable afterwards.
 *
 *  - Template parameters: T=data type (e.g. float),  N=dimension (e.g. 3),
 *    StoragePolicy:
 *      -# 'Owning' (default): uses the native memory model and has full ownership of the multi-dimensional data
 *      -# 'View': same memory model as 'owning', but without ownership: uses an externally-allocated 1-D data block
 *      -# 'Non-Contiguous View': uses externally-allocated non-contiguously allocated data
 *
 *  - Guarantees: Dynamic memory allocation only during construction and when calling subView()
 */
template<typename T, int N, class StoragePolicy = StoragePolicyOwning<T, N>>
class HyperBuffer
{
    using size_type                 = int;
    using pointer_type              = typename add_pointers_to_type<T, N>::type;
    using const_pointer_type        = typename add_const_pointers_to_type<T, N>::type;
    using subdim_pointer_type       = typename remove_pointers_from_type<pointer_type, 1>::type;
    using subdim_const_pointer_type = typename remove_pointers_from_type<const_pointer_type, 1>::type;
    
public:
    /** Generic Constructor that forwards all arguments to the storage policy constructor */
    template<typename... I>
    explicit HyperBuffer(I... i) : m_storage(i...) {}
    
    ~HyperBuffer() = default;
    
    // MARK: need explicit default copy/move ctors and assignment operators for this object to be fully copyable/moveable
    HyperBuffer(const HyperBuffer&) = default;
    HyperBuffer& operator=(const HyperBuffer&) = default;
    HyperBuffer(HyperBuffer&&) noexcept = default;
    HyperBuffer& operator= (HyperBuffer&&) noexcept = default;
    
    /**
     * Copy constructor from an object with a different storage policy. Enabled only Policy differs from current one
     * (to avoid hijacking the 'normal' copy constructor).
     * @attention This is dangerous. It's meant to allow a 'view' to be constructed from an 'owning' buffer only and is
     * implemented using a special ctor in the corresponding StoragePolicy.
     */
    template<typename U, int M, class AnotherStoragePolicy, typename std::enable_if<!std::is_same<AnotherStoragePolicy, StoragePolicy>::value, int>::type = 0>
    explicit HyperBuffer(HyperBuffer<U, M, AnotherStoragePolicy>& other) : m_storage(other.m_storage) {}
    
    // MARK: dimension extents
    int size(int i) const { return m_storage.size(i); }
    const std::array<int, N>& sizes() const noexcept { return m_storage.sizes(); }
    
    // MARK: data() -- raw pointer to beginning of underlying storage (pointers or data, depending on dimension)
    FOR_Nx const_pointer_type data() const noexcept { return m_storage.getDataPointer_Nx(); }
    FOR_Nx       pointer_type data()       noexcept { return m_storage.getDataPointer_Nx(); }
    FOR_N1           const T* data() const noexcept { return m_storage.getDataPointer_N1(); }
    FOR_N1                 T* data()       noexcept { return m_storage.getDataPointer_N1(); }
    
    // MARK: operator[] -- raw data/pointer access; returns pointer N>1, reference for N=1
    FOR_Nx subdim_const_pointer_type operator[] (size_type i) const { return m_storage.getDataPointer_Nx()[i]; }
    FOR_Nx       subdim_pointer_type operator[] (size_type i)       { return m_storage.getDataPointer_Nx()[i]; }
    FOR_N1                  const T& operator[] (size_type i) const { return m_storage.getDataPointer_N1()[i]; }
    FOR_N1                        T& operator[] (size_type i)       { return m_storage.getDataPointer_N1()[i]; }

    // MARK: at(...) -- Exists only for call with N parameters, returns data
    FOR_Nx_N const T& at(size_type dn, I... i) const { return createSubBuffer(dn).at(i...); }
    FOR_Nx_N       T& at(size_type dn, I... i)       { return createSubBuffer(dn).at(i...); }
    FOR_N1   const T& at(size_type i)          const { return m_storage.getDataPointer_N1()[i]; }
    FOR_N1         T& at(size_type i)                { return m_storage.getDataPointer_N1()[i]; }
    
    // MARK: subView(...) -- returns <T,N-1> instance
    FOR_Nx_V decltype(auto) subView(size_type dn, I... i) const { return createSubBuffer(dn).subView(i...); }
    FOR_Nx   decltype(auto) subView(size_type dn)         const { return createSubBuffer(dn); }
    
private:
    const HyperBuffer<T, N-1, typename StoragePolicy::SubBufferPolicy> createSubBuffer(size_type index) const
    {
        ASSERT(index < this->size(0), "Index out of range");
        auto subViewData = m_storage.getSubDimData(index);
        std::array<int, N-1> subViewDims = StdArrayOperations::shaveOffFirstElement(sizes());
        return HyperBuffer<T, N-1, typename StoragePolicy::SubBufferPolicy>(subViewData, subViewDims);
    }
    
    HyperBuffer<T, N-1, typename StoragePolicy::SubBufferPolicy> createSubBuffer(size_type index)
    {
        return std::as_const(*this).createSubBuffer(index);
    }
    
private:
    // Allow instances with other template arguments to access private members
    template<typename U, int M, class AnotherStoragePolicy> friend class HyperBuffer;
    
    StoragePolicy m_storage;
    
};

// MARK: Aliases (for convenience)

template<typename T, int N>
using HyperBufferView = HyperBuffer<T, N, StoragePolicyView <T, N>>;

template<typename T, int N>
using HyperBufferViewNC = HyperBuffer<T, N, StoragePolicyViewNonContiguous <T, N>>;





} // namespace slb
