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
    #define WARNINGS_OVERRIDE _Pragma("GCC diagnostic push")
    #define WARNINGS_RESTORE_ALL _Pragma("GCC diagnostic pop")
    #define  WARNINGS_DISABLE_SIGN_UNSIGNED \
        _Pragma("GCC diagnostic ignored \"-Wconversion\"") \
        _Pragma("GCC diagnostic ignored \"-Wsign-conversion\"") \
        _Pragma("GCC diagnostic ignored \"-Wsign-compare\"")
    #define WARNINGS_DISABLE_C_CAST _Pragma("GCC diagnostic ignored \"-Wold-style-cast\"")
    #define WARNINGS_DISABLE_CAST_ALIGN _Pragma("GCC diagnostic ignored \"-Wcast-align\"")

#elif defined(_MSC_VER)
    #define WARNINGS_OVERRIDE __pragma( warning(push) )
    #define WARNINGS_RESTORE_ALL __pragma( warning(pop) )
    #define WARNINGS_DISABLE_SIGN_UNSIGNED __pragma(warning( disable : 4245) )
    #define WARNINGS_DISABLE_C_CAST
    #define WARNINGS_DISABLE_CAST_ALIGN

#endif

namespace TestCommon
{
using stl_size_type = typename std::vector<float>::size_type;
constexpr stl_size_type STL(int i) { return static_cast<stl_size_type>(i); }

static inline std::vector<float> createRandomVector(int length, int seed=0)
{
    std::vector<float> result(STL(length));
    std::mt19937 engine(static_cast<unsigned>(seed));
    std::uniform_real_distribution<> dist(-1, 1); //(inclusive, inclusive)
    for (auto& sample : result) {
        sample = static_cast<float>(dist(engine));
    }
    return result;
}

static inline std::vector<int> createRandomVectorInt(int length, int seed=0)
{
    std::vector<int> result(STL(length));
    std::mt19937 engine(static_cast<unsigned>(seed));
    std::uniform_real_distribution<> dist(-1, 1); //(inclusive, inclusive)
    for (auto& sample : result) {
        sample = static_cast<int>(1000*dist(engine));
    }
    return result;
}



} // namespace TestCommon
