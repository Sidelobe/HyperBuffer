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

// MARK: - Variadic arguments / parameter pack helpers

/** Multiply parameter pack */
template <typename... Args>
auto multiplyArgs(Args... args) -> typename std::common_type<Args...>::type
{
    using unused = int[];
    typename std::common_type<Args...>::type result{1};
    (void)unused { 0, ( result *= args, 0 ) ... };
    return result;
}



// MARK: - DelArray  TO BE UPDATED---
template <bool isFinalType> struct DelArray;
template <> struct DelArray<true>;
template <> struct DelArray<false>;

using std::is_arithmetic;
using std::remove_pointer;
using std::is_pointer;

template <typename Cont>
void deleteArray(Cont arr, const size_t* sz, const size_t dimCtr=1)
{
    DelArray<is_arithmetic<typename remove_pointer<Cont>::type>::value>::delArray(arr, sz, dimCtr);
}

template <> struct DelArray<true> {
    template<typename Cont>
    static void delArray(Cont arr, const size_t* sz, const size_t dimCtr)
    {
        if (0 != sz[dimCtr-1]) {
            free(arr);
        }
    }
};
template <> struct DelArray<false> {
    template<typename Cont>
    static void delArray(Cont arr, const size_t* sz, const size_t dimCtr)
    {
        for (unsigned k=0; k<sz[dimCtr-1]; ++k) {
            deleteArray(arr[k], sz, dimCtr+1);
        }
        free(arr);
    }
};

// MARK: - CallocArray TO BE UPDATED---
template <bool isFinalType> struct CallocArray;
template <> struct CallocArray<true>;
template <> struct CallocArray<false>;

template <typename Cont>
Cont callocArray(const size_t* sz, const size_t dimCtr=1)
{
    return CallocArray<std::is_arithmetic<typename std::remove_pointer<Cont>::type>::value>::template memCallocArray<Cont>(sz, dimCtr);
}

template <> struct CallocArray<true> {
    template<typename Cont>
    static Cont memCallocArray(const size_t* sz, const size_t dimCtr)
    {
        return (Cont)std::calloc(sz[dimCtr-1], sizeof(typename std::remove_pointer<Cont>::type));
    }
};
template <> struct CallocArray<false> {
    template<typename Cont>
    static Cont memCallocArray(const size_t* sz, const size_t dimCtr)
    {
        Cont arr = (Cont)calloc(sz[dimCtr-1], sizeof(typename std::remove_pointer<Cont>::type));
        for (unsigned k=0; k<sz[dimCtr-1]; ++k) {
            arr[k] = callocArray<typename std::remove_pointer<Cont>::type>(sz, dimCtr+1);
        }
        return arr;
    }
};
