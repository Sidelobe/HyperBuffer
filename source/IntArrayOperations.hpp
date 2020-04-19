//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include "TemplateUtils.hpp"

namespace IntArrayOperations
{

/** Calculates the sum of the elements of the supplied array */
template<std::size_t N>
constexpr int sum(const int (&array)[N])
{
    auto intTuple = VarArgOperations::makeIntTuple(array);
    return VarArgOperations::apply([](auto&&... args)
    {
        return VarArgOperations::sum(std::forward<decltype(args)>(args)...);
    }, intTuple);
}

/** Calculates the sum of the elements in range [start, end[ of the supplied array */
template<std::size_t N>
constexpr int sumOverRange(int begin, int end, const int (&array)[N])
{
    auto intTuple = VarArgOperations::makeIntTuple(array);
    return VarArgOperations::apply([begin, end](auto&&... args)
    {
        return VarArgOperations::sumOverRange(begin, end, std::forward<decltype(args)>(args)...);
    }, intTuple);
}
/** Calculates the sum of the elements in range [0, cap[ of the supplied array */
template<std::size_t N>
constexpr int sumCapped(int cap, const int (&array)[N])
{
    auto intTuple = VarArgOperations::makeIntTuple(array);
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return VarArgOperations::sumCapped(cap, std::forward<decltype(args)>(args)...);
    }, intTuple);
}

/** Calculates the product of the elements of the supplied array */
template<std::size_t N>
constexpr int product(const int (&array)[N])
{
    auto intTuple = VarArgOperations::makeIntTuple(array);
    return VarArgOperations::apply([](auto&&... args)
    {
        return VarArgOperations::product(std::forward<decltype(args)>(args)...);
    }, intTuple);
}

/** Calculates the product of the elements in range [start, end[ of the supplied array */
template<std::size_t N>
constexpr int productOverRange(int begin, int end, const int (&array)[N])
{
    auto intTuple = VarArgOperations::makeIntTuple(array);
    return VarArgOperations::apply([begin, end](auto&&... args)
    {
        return VarArgOperations::productOverRange(begin, end, std::forward<decltype(args)>(args)...);
    }, intTuple);
}

/** Calculates the product of the elements in range [0, cap[ of the supplied array */
template<std::size_t N>
constexpr int productCapped(int cap, const int (&array)[N])
{
    auto intTuple = VarArgOperations::makeIntTuple(array);
    return VarArgOperations::apply([cap](auto&&... args)
    {
        return VarArgOperations::productCapped(cap, std::forward<decltype(args)>(args)...);
    }, intTuple);
}

} // namespace IntArrayOperations



