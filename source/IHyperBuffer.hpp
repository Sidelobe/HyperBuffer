//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <memory>
#include <vector>

#include "TemplateUtils.hpp"

// TODO: REMOVE THIS?
template<class T>
struct ArrayBaseType
{
  using type = T;
};
template<class T, size_t n>
struct ArrayBaseType<T[n]>
{
    using type = typename ArrayBaseType<T>::type;
};

#define TypeOfArrayBase(T) ArrayBaseType<T>::type


// MARK: - HyperBuffer Interface
template<typename T, int N>
class IHyperBuffer
{
protected:
    using size_type = int;
    using pointer_type = typename add_pointers_to_type<T, N>::type;

public:
    // TODO:
};

