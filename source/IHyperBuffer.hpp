//
//  ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
//  ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
//  ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─
//
//  © 2020 Lorenz Bucher - all rights reserved

#include <memory>
#include <vector>

#include "TemplateUtils.hpp"

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
//    template<int... I>
//    decltype(auto) operator() (size_type i...) = 0;
//
    virtual pointer_type data() const {}
    virtual pointer_type at(size_type offset) const {}
    
    virtual pointer_type getDataPointers() const = 0;
    
    virtual int* dims() const {}
    virtual int dim(int i) const {}
};

