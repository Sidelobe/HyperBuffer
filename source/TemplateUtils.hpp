//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2023 Lorenz Bucher - all rights reserved
//  https://github.com/Sidelobe/HyperBuffer

#pragma once

#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <type_traits>

#define UNUSED(x) (void)x

/* Macro to detect if exceptions are disabled (works on GCC, Clang and MSVC) 3 */
#ifndef __has_feature
#define __has_feature(x) 0
#endif
#if !__has_feature(cxx_exceptions) && !defined(__cpp_exceptions) && !defined(__EXCEPTIONS) && !defined(_CPPUNWIND)
  #define EXCEPTIONS_DISABLED
#endif

// GCC
//  Bug 67371 - Never executed "throw" in constexpr function fails to compile
//  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=86678
//  Fixed for GCC 9.
#if defined(__GNUC__) && (__GNUC__ < 9)
#   define BUG_67371
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
#ifdef BUG_67371
static inline void handleAssert(const char* conditionAsText, bool condition, const char* file, int line, const char* message = "")
#else
static constexpr void handleAssert(const char* conditionAsText, bool condition, const char* file, int line, const char* message = "")
#endif
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
    using type = typename remove_pointers_from_type<typename std::remove_pointer_t<T>, N-1>::type;
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
    return sizeof(a) / sizeof(typename std::remove_all_extents_t<T>);
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
