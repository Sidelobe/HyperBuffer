//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <array>

#include "CompiletimeMath.hpp"
#include "TemplateUtils.hpp"

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
