//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <algorithm>
#include <cstdlib>
#include <type_traits>


// MARK: - Add pointers to type
// Recursive Template trick to add an arbitrary number of pointers to a type
template<class T,int N>
struct add_pointers_to_type
{
  using type = typename add_pointers_to_type<T*,N-1>::type;
};

template<class T>
struct add_pointers_to_type<T,0>
{
  using type = T;
};

static_assert(std::is_same<add_pointers_to_type<int,1>::type, int*>{}, "");
static_assert(std::is_same<add_pointers_to_type<float,3>::type, float***>{}, "");
static_assert(std::is_same<add_pointers_to_type<float,0>::type, float>{}, "");

// MARK: - Remove pointers from type
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

// MARK: - Variadic arguments / parameter pack helpers

namespace VarArgOperations
{
/** Calculate the sum of all args in parameter pack */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr int sum(Args... args)
{
    using unused = int[];
    T result{0};
    (void)unused { 0, ( result += args, 0 ) ... };
    return result;
}

/** Multiply all args in parameter pack - until 'cap' index */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr int productCapped(int maxLength, Args... args)
{
    assert(maxLength > 0 && "Cannot cap a length-one argument list");
    int product{1};
    T values[]{ args... };
    for (int i=0; i < maxLength; ++i) {
        product *= values[i];
    }
    return product;
}

/** Multiply all args in parameter pack */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr int product(Args... args)
{
    using unused = int[];
    T result{1};
    (void)unused { 0, ( result *= args, 0 ) ... };
    return result;
}

/** Calculate the cumulative product of args in parameter pack - until 'cap' index */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr int sumOfCumulativeProductCapped(int maxLength, Args... args)
{
    assert(maxLength > 0 && "Cannot cap a length-one argument list");
    int sum{0};
    T values[]{ args... };
    for (int i=0; i < maxLength; ++i) {
        int cumulativeProduct{1};
        for (int j=0; j <= i; ++j) {
            cumulativeProduct *= values[j];
        }
        sum += cumulativeProduct;
    }
    return sum;
}

/** Calculate the cumulative product of all args in parameter pack */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr int sumOfCumulativeProduct(Args... args)
{
    return sumOfCumulativeProductCapped(sizeof...(Args), args...);
}


// Make Tuple from std::array
template<std::size_t... I, std::size_t N>
constexpr auto makeIntTuple(const std::array<int, N>& arr, std::index_sequence<I...>) {
    return std::make_tuple(arr[I]...);
}

template<std::size_t N>
constexpr auto makeIntTuple(const std::array<int, N>& arr) {
    return makeIntTuple(arr, std::make_index_sequence<N>{});
}

// Make Tuple from Raw Array
template<std::size_t... I, std::size_t N>
constexpr auto makeIntTuple(const int (&arr)[N], std::index_sequence<I...>) {
    return std::make_tuple(arr[I]...);
}

template<std::size_t N>
constexpr auto makeIntTuple(const int (&arr)[N]) {
    return makeIntTuple(arr, std::make_index_sequence<N>{});
}


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
