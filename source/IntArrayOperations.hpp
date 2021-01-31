//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <array>

#include "TemplateUtils.hpp"

// ---------------------------------------------------------------------------------------------------------------------
// These are runtime function-wrappers designed to allow the use of the compile-time utility functions in VarArgOperations::
// ---------------------------------------------------------------------------------------------------------------------
namespace slb
{

// MARK: - C-Style Arrays
namespace IntArrayOperations
{

/** Calculates the sum of the elements of the supplied array */
template<std::size_t N>
constexpr int sum(const int (&array)[N])
{
    return VarArgOperations::apply([](auto&&... args)
    {
        return VarArgOperations::sum(std::forward<decltype(args)>(args)...);
    }, VarArgOperations::makeIntTuple(array));
}

/** Calculates the sum of the elements in range [start, end[ of the supplied array */
template<std::size_t N>
constexpr int sumOverRange(int begin, int end, const int (&array)[N])
{
    return VarArgOperations::apply([begin, end](auto&&... args)
    {
        return VarArgOperations::sumOverRange(begin, end, std::forward<decltype(args)>(args)...);
    }, VarArgOperations::makeIntTuple(array));
}
/** Calculates the sum of the elements in range [0, cap[ of the supplied array */
template<std::size_t N>
constexpr int sumCapped(int cap, const int (&array)[N])
{
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return VarArgOperations::sumCapped(cap, std::forward<decltype(args)>(args)...);
    },  VarArgOperations::makeIntTuple(array));
}

/** Calculates the product of the elements of the supplied array */
template<std::size_t N>
constexpr int product(const int (&array)[N])
{
    return VarArgOperations::apply([](auto&&... args)
    {
        return VarArgOperations::product(std::forward<decltype(args)>(args)...);
    }, VarArgOperations::makeIntTuple(array));
}

/** Calculates the product of the elements in range [start, end[ of the supplied array */
template<std::size_t N>
constexpr int productOverRange(int begin, int end, const int (&array)[N])
{
    return VarArgOperations::apply([begin, end](auto&&... args)
    {
        return VarArgOperations::productOverRange(begin, end, std::forward<decltype(args)>(args)...);
    }, VarArgOperations::makeIntTuple(array));
}

/** Calculates the product of the elements in range [0, cap[ of the supplied array */
template<std::size_t N>
constexpr int productCapped(int cap, const int (&array)[N])
{
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return VarArgOperations::productCapped(cap, std::forward<decltype(args)>(args)...);
    }, VarArgOperations::makeIntTuple(array));
}

/** Calculates sum of the cumulative product of the supplied array */
template<std::size_t N>
constexpr int sumOfCumulativeProduct(const int (&array)[N])
{
    return VarArgOperations::apply([](auto&&... args)
    {
        return VarArgOperations::sumOfCumulativeProduct(std::forward<decltype(args)>(args)...);
    }, VarArgOperations::makeIntTuple(array));
}

/** Calculates sum of the cumulative product in range [0, cap[ of the supplied array */
template<std::size_t N>
constexpr int sumOfCumulativeProductCapped(int cap, const int (&array)[N])
{
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return VarArgOperations::sumOfCumulativeProductCapped(cap, std::forward<decltype(args)>(args)...);
    }, VarArgOperations::makeIntTuple(array));
}


} // namespace IntArrayOperations

// MARK: - std::array
namespace StdArrayOperations
{

/** Calculates the sum of the elements of the supplied array */
template<std::size_t N>
constexpr int sum(const std::array<int, N>& array)
{
    return VarArgOperations::apply([](auto&&... args)
    {
        return VarArgOperations::sum(std::forward<decltype(args)>(args)...);
    }, array);
}

/** Calculates the sum of the elements in range [start, end[ of the supplied array */
template<std::size_t N>
constexpr int sumOverRange(int begin, int end, const std::array<int, N>& array)
{
    return VarArgOperations::apply([begin, end](auto&&... args)
    {
        return VarArgOperations::sumOverRange(begin, end, std::forward<decltype(args)>(args)...);
    }, array);
}
/** Calculates the sum of the elements in range [0, cap[ of the supplied array */
template<std::size_t N>
constexpr int sumCapped(int cap, const std::array<int, N>& array)
{
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return VarArgOperations::sumCapped(cap, std::forward<decltype(args)>(args)...);
    },  array);
}

/** Calculates the product of the elements of the supplied array */
template<std::size_t N>
constexpr int product(const std::array<int, N>& array)
{
    return VarArgOperations::apply([](auto&&... args)
    {
        return VarArgOperations::product(std::forward<decltype(args)>(args)...);
    }, array);
}

/** Calculates the product of the elements in range [start, end[ of the supplied array */
template<std::size_t N>
constexpr int productOverRange(int begin, int end, const std::array<int, N>& array)
{
    return VarArgOperations::apply([begin, end](auto&&... args)
    {
        return VarArgOperations::productOverRange(begin, end, std::forward<decltype(args)>(args)...);
    }, array);
}

/** Calculates the product of the elements in range [0, cap[ of the supplied array */
template<std::size_t N>
constexpr int productCapped(int cap, const std::array<int, N>& array)
{
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return VarArgOperations::productCapped(cap, std::forward<decltype(args)>(args)...);
    }, array);
}

/** Calculates sum of the cumulative product of the supplied array */
template<std::size_t N>
constexpr int sumOfCumulativeProduct(const std::array<int, N>& array)
{
    return VarArgOperations::apply([](auto&&... args)
    {
        return VarArgOperations::sumOfCumulativeProduct(std::forward<decltype(args)>(args)...);
    }, array);
}

/** Calculates sum of the cumulative product in range [0, cap[ of the supplied array */
template<std::size_t N>
constexpr int sumOfCumulativeProductCapped(int cap, const std::array<int, N>& array)
{
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return VarArgOperations::sumOfCumulativeProductCapped(cap, std::forward<decltype(args)>(args)...);
    }, array);
}

/** @returns the N-1 sub-array : removes / "shaves off" the first element */
template<std::size_t N>
static constexpr std::array<int, N-1> shaveOffFirstElement(const std::array<int, N>& array)
{
    std::array<int, N-1> subarray;
    for (int i=0; i < N-1; ++i) {
        subarray[i] = array[i+1];
    }
    return  subarray;
}

} // namespace StdArrayOperations

} // namespace slb
