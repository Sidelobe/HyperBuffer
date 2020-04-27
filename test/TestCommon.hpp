//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#pragma once

#include <catch2/catch.hpp>

#define STRINGIFY(a) #a
// Turn off sign warnings in Tests
#if defined(__clang__) || defined(__GNUC__)
    #define DISABLE_SIGN_UNSIGNED_WARNINGS \
        _Pragma("clang diagnostic push") \
        _Pragma("clang diagnostic ignored \"-Wconversion\"") \
        _Pragma("clang diagnostic ignored \"-Wsign-conversion\"")
    #define RESTORE_SIGN_UNSIGNED_WARNINGS \
         _Pragma("clang diagnostic pop")

#elif defined(_MSC_VER)
    #define DISABLE_SIGN_UNSIGNED_WARNINGS \
        __pragma(warning(suppress : 4245))
    #define RESTORE_SIGN_UNSIGNED_WARNINGS
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
