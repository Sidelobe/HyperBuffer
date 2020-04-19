```
 ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
 ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
 ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─

```

### A C++ structure to allocate and/or access multi-dimensional data

![](https://img.shields.io/badge/dependencies-C++14,_STL-brightgreen)
![](https://img.shields.io/badge/source-header--only-brightgreen)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
[![Build Status (master)](https://travis-ci.com/Sidelobe/HyperBuffer.svg?branch=master)](https://travis-ci.com/Sidelobe/HyperBuffer)
[![Build Status (develop)](https://travis-ci.com/Sidelobe/HyperBuffer.svg?branch=develop)](https://travis-ci.com/Sidelobe/HyperBuffer)

The main use case for this container is to hold dynamically-allocated N-dimensional datasets in memory and provide convenient access to it, while minimizing performance/memory overhead and unnecessary dynamic allocation.

### Main design paradigms and parameters:

* **data types**: defined at compile-time (template argument), identical for all dimensions
* **number of dimensions** : defined at compile-time (template argument)
* **size of these dimensions**: defined at run-time, but cannot be changed once the object is constructed: `HyperBuffer` is non-resizable.

>NOTE: All dimensions are uniform, i.e. each 'slice' in a given dimension has equal length.

* no allocation after construction
* 


```

TBD : code examples











```

#### Requirements / Compatibility
 - C++14, STL
 - tested with GCC 7 and clang 11


 
### Data Access Modes
There are several

#### Owning

#### Pre-Allocated Multi-Dimensional

#### Pre-Allocated Flat


<<<<<<< HEAD
C++ structure to manage multi-dimensional data
=======
### A C++ structure to allocate and/or access multi-dimensional data

![](https://img.shields.io/badge/dependencies-C++14,_STL-brightgreen)
![](https://img.shields.io/badge/source-header--only-brightgreen)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
[![Build Status (master)](https://travis-ci.com/Sidelobe/HyperBuffer.svg?branch=master)](https://travis-ci.com/Sidelobe/HyperBuffer)
[![Build Status (develop)](https://travis-ci.com/Sidelobe/HyperBuffer.svg?branch=develop)](https://travis-ci.com/Sidelobe/HyperBuffer)

The main use case for this container is to hold dynamically-allocated N-dimensional datasets in memory and provide convenient access to it, while minimizing performance/memory overhead and unnecessary dynamic allocation.

### Main design paradigms and parameters:

* **data types**: defined at compile-time (template argument), identical for all dimensions
* **number of dimensions** : defined at compile-time (template argument)
* **size of these dimensions**: defined at run-time, but cannot be changed once the object is constructed: `HyperBuffer` is non-resizable.

>NOTE: All dimensions are uniform, i.e. each 'slice' in a given dimension has equal length.

* no allocation after construction - allocation free move() semantics
 

```

TBD : code examples











```

#### Requirements / Compatibility
 - C++14, STL
 - tested with GCC 7 and clang 11


 
### Data Access Modes
There are several

#### Owning

NOTE: the default constructor std::vector<> will allocate in some STL implementations 

The Microsoft Visual Studio Compiler (MSVC) allocates dynamic memory in the default constructors of some STL containers.
This leads to unwanted allocation in move semantics, where the default constructor is implicitly called by the compiler at some point in the process. This has pushed us to avoid using STL containers in these scenarios.
 
According to [https://stackoverflow.com/a/48744563/649700]() this happens when `_ITERATOR_DEBUG_LEVEL` is set to 1 or 2, which is the default setting for debug builds.
 
[https://docs.microsoft.com/en-us/cpp/standard-library/iterator-debug-level?view=vs-2019]()


#### Pre-Allocated Multi-Dimensional

#### Pre-Allocated Flat

### Memory Overhead 
The memory overhead of an N-Dimensional `HyperBuffer` of `base_type` is given by this formula:
`D1 * sizeof(base_type) + (D2 * ... * DN) * sizeof(pointer_type)`

For a `HyperBuffer<float, 3>(2, 4, 5)` this would mean
`5 * sizeof(float) + 2 * 4 * sizeof(float*)` -- on my 64-bit machine that is 84 Bytes. The lowest dimension defines the data storage; the higher dimensions just store a pointer for every element.



>>>>>>> ed9f245... updated HyperBuffer
