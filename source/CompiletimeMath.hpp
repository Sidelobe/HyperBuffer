//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <type_traits>

namespace slb
{

// MARK: - Variadic arguments / parameter pack helpers -- [resolved at compile time!]
namespace CompiletimeMath
{

// MARK: - Sum
/** Calculate the sum of a given number of args in parameter pack - starting from the given firstSummand (1-based) */
template<typename... Args, typename T = typename std::common_type<Args...>::type>
constexpr T sumOverRange(int firstSummand, int numSummands, Args... args) noexcept
{
    firstSummand = std::max<int>(firstSummand, 1);
    numSummands = std::min<int>(numSummands, sizeof...(args) - firstSummand + 1);
    
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
    if (numFactors <= 0) { return 0; }
    firstFactor = std::max<int>(firstFactor, 1);
    numFactors = std::min<int>(numFactors, sizeof...(args) - firstFactor + 1);
    
    T product{1};
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
    numElements = std::min<int>(numElements, sizeof...(args) - firstElement + 1);
    
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
