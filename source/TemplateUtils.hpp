//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <type_traits>

#define UNUSED(x) (void)x

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
 * NOTE: this assertion handler is constexpr - to allow its use inside constexpr functions.
 * The handler will still be evaluated at runtime, but memory is only allocated IF the assertion is triggered.
 */
static constexpr void handleAssert(const char* conditionAsText, bool condition, const char* file, int line, const char* message = "")
{
    if (condition == false) {
        throw std::runtime_error(std::string("Assertion failed: ") + conditionAsText + " (" + file + ":" + std::to_string(line) + ") " + message);
    }
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

// MARK: - Variadic arguments / parameter pack helpers -- [resolved at compile time!]
namespace VarArgOperations
{

// MARK: - Sum
/** Calculate the sum of all args in parameter pack - in range [start, end[ */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumOverRange(int begin, int end, Args... args)
{
    ASSERT(begin >= 0);
    ASSERT(static_cast<unsigned>(end) <= sizeof...(args));
    ASSERT(end-begin > 0, "Cannot cap a length-one argument list");
    T sum{0};
    T values[]{ args... };
    for (int i=begin; i < end; ++i) {
        sum += values[i];
    }
    return sum;
}

/** Calculate the sum of all args in parameter pack - until before 'cap' index */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumCapped(int cap, Args... args)
{
    ASSERT(cap > 0);
    return sumOverRange(0, cap, args...);
}

/** Calculate the sum of all args in parameter pack */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sum(Args... args)
{
    return sumCapped(sizeof...(args), args...);
}

// MARK: - Product

/** Calculate the product of all args in parameter pack - in range [start, end[ */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T productOverRange(int begin, int end, Args... args)
{
    ASSERT(begin >= 0);
    ASSERT(static_cast<unsigned>(end) <= sizeof...(args));
    ASSERT(end-begin > 0, "Cannot cap a length-one argument list");
    T product{1};
    T values[]{ args... };
    for (int i=begin; i < end; ++i) {
        product *= values[i];
    }
    return product;
}

/** Calculates the product of all args in parameter pack - until before 'cap' index */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T productCapped(int cap, Args... args)
{
    return productOverRange(0, cap, args...);
}

/** Multiply all args in parameter pack */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T product(Args... args)
{
    return productCapped(sizeof...(args), args...);
}

// MARK: - Sum of Cumulative Product

/** Calculate the cumulative product of args in parameter pack - - in range [start, end[ */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumOfCumulativeProductOverRange(int begin, int end, Args... args)
{
    ASSERT(begin >= 0);
    ASSERT(static_cast<unsigned>(end) <= sizeof...(args));
    if (end-begin == 0) {
        return 0;
    }
    T sum{0};
    T values[]{ args... };
    for (int i=begin; i < end; ++i) {
        T cumulativeProduct{1};
        for (int j=0; j <= i; ++j) {
            cumulativeProduct *= values[j];
        }
        sum += cumulativeProduct;
    }
    return sum;
}

/** Calculate the cumulative product of args in parameter pack - until before 'cap' index */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumOfCumulativeProductCapped(int cap, Args... args)
{
    return sumOfCumulativeProductOverRange(0, cap, args...);
}

/** Calculate the cumulative product of all args in parameter pack */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumOfCumulativeProduct(Args... args)
{
    return sumOfCumulativeProductCapped(sizeof...(Args), args...);
}

// MARK: - Tuple creation
// Make Tuple from std::array
template<std::size_t... I, std::size_t N>
constexpr auto makeIntTuple(const std::array<int, N>& arr, std::index_sequence<I...>)
{
    return std::make_tuple(arr[I]...);
}

template<std::size_t N>
constexpr auto makeIntTuple(const std::array<int, N>& arr)
{
    return makeIntTuple(arr, std::make_index_sequence<N>{});
}

// Make Tuple from Raw Array
template<std::size_t... I, std::size_t N>
constexpr auto makeIntTuple(const int (&arr)[N], std::index_sequence<I...>)
{
    return std::make_tuple(arr[I]...);
}

template<std::size_t N>
constexpr auto makeIntTuple(const int (&arr)[N])
{
    return makeIntTuple(arr, std::make_index_sequence<N>{});
}


// MARK: std::apply - like implementation
/**
 * Helper to call a function with a std::tuple of arguments (standardized in C++17 as std::apply)
 * source: https://www.programming-books.io/essential/cpp/iterating-with-stdinteger-sequence-cca589107b7a499e9e7275427a994f97
 */
namespace detail
{
    template <class F, class Tuple, std::size_t... Is>
    decltype(auto) apply_impl(F&& f, Tuple&& tpl, std::index_sequence<Is...> )
    {
        return std::forward<F>(f)(std::get<Is>(std::forward<Tuple>(tpl))...);
    }
}

// apply from std::tuple
template <class F, class Tuple>
decltype(auto) apply(F&& f, Tuple&& tpl)
{
    return detail::apply_impl(std::forward<F>(f),
        std::forward<Tuple>(tpl),
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{});
}

// apply from std::array (directly)
template <class F, std::size_t N>
decltype(auto) apply(F&& f, const std::array<int, N>& arr)
{
    return detail::apply_impl(std::forward<F>(f),
        makeIntTuple(arr),
        std::make_index_sequence<N>{});
}

} // namespace VarArgOperations


} // namespace slb
