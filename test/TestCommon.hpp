//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <catch2/catch.hpp>

// Turn off some warnings in tests -- Otherwise some things just become very annoying
// Clang accepts gcc diagnostic, not the other way around
#if defined(__clang__) || defined(__GNUC__)
    #define WARNINGS_OVERRIDE _Pragma("gcc diagnostic push")
    #define WARNINGS_RESTORE_ALL _Pragma("gcc diagnostic pop")
    #define  WARNINGS_DISABLE_SIGN_UNSIGNED \
        _Pragma("gcc diagnostic ignored \"-Wconversion\"") \
        _Pragma("gcc diagnostic ignored \"-Wsign-conversion\"") \
        _Pragma("gcc diagnostic ignored \"-Wsign-compare\"")
    #define WARNINGS_DISABLE_C_CAST _Pragma("gcc diagnostic ignored \"-Wold-style-cast\"")
    #define WARNINGS_DISABLE_CAST_ALIGN _Pragma("gcc diagnostic ignored \"-Wcast-align\"")

#elif defined(_MSC_VER)
    #define WARNINGS_OVERRIDE __pragma( warning(push) )
    #define WARNINGS_RESTORE_ALL __pragma( warning(pop) )
    #define WARNINGS_DISABLE_SIGN_UNSIGNED __pragma(warning( disable : 4245) )
    #define WARNINGS_DISABLE_C_CAST
    #define WARNINGS_DISABLE_CAST_ALIGN

#endif

namespace TestCommon
{

template<typename T = float>
static inline std::vector<T> createRandomVector(unsigned length, unsigned seed=0)
{
    std::vector<T> result(length);
    std::mt19937 engine(seed);
    std::uniform_real_distribution<> dist(-1, 1); //(inclusive, inclusive)
    for (auto& sample : result) {
        sample = static_cast<float>(dist(engine));
    }
    return result;
}



} // namespace TestCommon
