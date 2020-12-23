```
 ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
 ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
 ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─

```

### A C++ structure to manage multi-dimensional data efficiently and safely

![](https://img.shields.io/github/license/Sidelobe/Hyperbuffer)
![](https://img.shields.io/badge/C++14-header--only-blue.svg?style=flat&logo=c%2B%2B)
![](https://img.shields.io/badge/dependencies-STL_only-blue)

The main use case for this container is to hold dynamically-allocated N-dimensional datasets in memory and provide convenient access to it, while minimizing performance/memory overhead and avoid unnecessary dynamic allocation.

## Usage Example
```
<TBD> : code examples







```

#### Requirements / Compatibility

 - C++14, STL only
 - Compiled & Tested with:
 	- GCC/g++ 9.3.0 (Linux)
	- Clang 11 (Xcode 11.3, macos)
	- MSVC++ 14.1 (Visual Studio 2017, Windows) 

## Design paradigms:

* **data types**: defined at compile-time (template argument), identical for all dimensions
* **number of dimensions**: defined at compile-time (template argument)
* **extent of dimensions**: can be defined at **run-time**, but cannot be changed once the object is constructed: `HyperBuffer` is non-resizable.

>**Note**: For the time being, I've constrained all dimensions to be uniform, i.e. each 'slice' in a given dimension has equal length.

API features:

* can provide a (multi-dimensional) raw pointer (e.g. `float***`) to highest-order or any sub-dimension
* supports `operator[]` to access the top-most dimension - can be chained: `h[3][0][6]`
* supports variadic `operator(...)` to allow access to any dimension: `h(3, 0, 6)` or `h(3, 0)`

Memory management details:

* no dynamic allocation after construction
* dynamic allocation-free move() semantics
* (*planned*) alignment of the data (lowest-order/innermost dimension) can be specified ('owning' mode only)
 
### Data Storage / Ownership Variants
There are 3 variants of `HyperBuffer`:

1. `HyperBuffer`: internally allocates the memory for the dimensions it was configured for.
1. `HyperBufferPreAlloc`: wraps around already-allocated multi-dimensional data and pointers. No dynamic memory allocation.
1. `HyperBufferPreAllocFlat`: uses an already-allocated memory area for the data, but manages it using its internal memory model. Only allocates memory for the pointers.

>**Note**: Behaviour on copy & move: `HyperBuffer` copies/moves the data like a normal object with data ownership. When copying `HyperBufferPreAlloc` and `HyperBufferPreAllocFlat`, however, the data is not duplicated - the copy references the original data as well.

### Build Status / Quality Metrics

![](https://img.shields.io/badge/branch-master-blue)
[![Build Status (master)](https://travis-ci.com/Sidelobe/HyperBuffer.svg?branch=master)](https://travis-ci.com/Sidelobe/HyperBuffer)
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
![](https://img.shields.io/badge/branch-develop-blue)
[![Build Status (develop)](https://travis-ci.com/Sidelobe/HyperBuffer.svg?branch=develop)](https://travis-ci.com/Sidelobe/HyperBuffer)

[![Sonarcloud Status](https://sonarcloud.io/api/project_badges/measure?project=Sidelobe_HyperBuffer&metric=alert_status)](https://sonarcloud.io/dashboard?id=Sidelobe_HyperBuffer) 
[![SonarCloud Coverage](https://sonarcloud.io/api/project_badges/measure?project=Sidelobe_HyperBuffer&metric=coverage)](https://sonarcloud.io/component_measures/metric/coverage/list?id=Sidelobe_HyperBuffer)
[![SonarCloud Bugs](https://sonarcloud.io/api/project_badges/measure?project=Sidelobe_HyperBuffer&metric=bugs)](https://sonarcloud.io/component_measures/metric/reliability_rating/list?id=Sidelobe_HyperBuffer)
[![SonarCloud Vulnerabilities](https://sonarcloud.io/api/project_badges/measure?project=Sidelobe_HyperBuffer&metric=vulnerabilities)](https://sonarcloud.io/component_measures/metric/security_rating/list?id=Sidelobe_HyperBuffer)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=Sidelobe_HyperBuffer&metric=ncloc)](https://sonarcloud.io/dashboard?id=Sidelobe_HyperBuffer)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=Sidelobe_HyperBuffer&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=Sidelobe_HyperBuffer)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=Sidelobe_HyperBuffer&metric=reliability_rating)](https://sonarcloud.io/dashboard?id=Sidelobe_HyperBuffer)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=Sidelobe_HyperBuffer&metric=security_rating)](https://sonarcloud.io/dashboard?id=Sidelobe_HyperBuffer)
[![Technical Debt](https://sonarcloud.io/api/project_badges/measure?project=Sidelobe_HyperBuffer&metric=sqale_index)](https://sonarcloud.io/dashboard?id=Sidelobe_HyperBuffer)

## Details: Internal Memory Model
In C++/C, there are two common ways of allocating a multi-dimensional data structure:

1. **contiguous / linear**: e.g. C-Style `int[2][3][5]`, which is just 'view' for a 1D `2*3*5` int array. All dimensions have to be uniform, alignment is achieved through padding. Other than this, there is zero memory overhead, unless you need to produce an `int***` to the data.

1. **linked / pointer**: arrays of pointers that point to other pointers and eventually the data. Dimensions can be non-uniform, data alignment can be achieved when allocating the innermost dimension.

Essentially, with method \#2 we differentiate between the memory required for pointers and the memory used for the actual data, which is located at the lowest dimension. Allocating the memory for the pointers usually involves individual allocation on every dimension recursively.

This also entails recursive (de-)allocation when copying or moving such a multidimensional data structure. For this reason, a linear memory model was chosen in this project, where both the data and all the pointers are stored linearly in a 'flat' array each.

This way, only 2 one-dimensional arrays (or similar data structure) need be allocated, regardless of the order of dimensions: one for the data, and one for the pointers.

#### Self-Referencing Pointer Array
This array contains the entire pointer data required for a multi-dimensional buffer. The pointers are stored contiguously within every dimension, which allows for proper dereferencing / arithmetics when accessing the data through them. The pointers for each dimension point to the respective pointers in the next dimension, except for the 2nd-to-lowest dimension pointers, which point at the data.

Here's an example for a 5-Dimensional buffer with the extents `{2, 3, 2, 3, 6}`. The indices (0-55) represent the position of these pointers in pointer array.

![](docu/PointerArrayGeometry.png)

### Memory Overhead & Buffer Geometry
The memory overhead of an N-Dimensional `HyperBuffer` of `base_type` has 2 components:

1. The amount of memory required for the **data** is given by the *product of all its dimension extents*. This corresponds to the amount of elements in the innermost dimension.

1. The **amount of pointers** need for a given multi-dimensional buffer is given by the *sum of the cumulative product of its dimension extents*, where with 'dimensions' we mean the 'pointer dimensions', i.e. we ignore the innermost dimension.

For a `HyperBuffer<float, 3>(2, 4, 5)` this would mean:

* `2*4*5 * sizeof(float) = 160 Bytes` for the data
* `(2 + 2*4) * sizeof(float*) = 80 Bytes` for the pointers (on a 64-bit machine)


## Lessons Learned: Unwanted Dynamic Memory (De-)Allocation

Since we could potentially use any data structure for both the pointers and data, `std::vector` is an obvious candidate. Defining a custom allocator would give us control over the allocation per se, but not over whether or when the allocator's `allocate()` function is called. As it turns out, the default constructor `std::vector` will allocate in some STL implementations, and not in others)!

This means that we have to manually define copy and move constructors and assignment operators for our class and make sure the default constructor is NEVER called. This is not trivial


The Microsoft Visual Studio Compiler (MSVC) allocates dynamic memory in the default constructors of some STL containers. This leads to unwanted allocation in move semantics, where the default constructor is implicitly called by the compiler at some point in the process. This has pushed us to avoid using STL containers in these scenarios.

 According to [https://stackoverflow.com/a/48744563/649700]() this happens when `_ITERATOR_DEBUG_LEVEL` is set to 1 or 2, which is the default setting for debug builds. Also cf. [https://docs.microsoft.com/en-us/cpp/standard-library/iterator-debug-level?view=vs-2019]()

To my knowledge, if you change `_ITERATOR_DEBUG_LEVEL` however, you have to change it globally within your build -- you cannot link items compiled with different `_ITERATOR_DEBUG_LEVEL` settings.
 


