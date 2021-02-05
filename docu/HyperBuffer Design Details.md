```
 ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
 ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
 ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─

```

### A C++ structure to manage multi-dimensional data efficiently and safely

![](https://img.shields.io/github/license/Sidelobe/Hyperbuffer)
![](https://img.shields.io/badge/C++14-header--only-blue.svg?style=flat&logo=c%2B%2B)
![](https://img.shields.io/badge/dependencies-STL_only-blue)

# Design Details: Internal Memory Model
In C++/C, there are two common ways of allocating a multi-dimensional data structure:

1. **contiguous / linear**: e.g. C-Style `int[2][3][5]`, which is just 'view' for a 1D `2*3*5` int array. All dimensions have to be uniform, alignment is achieved through padding. Other than this, there is zero memory overhead, unless you need to produce an `int***` to the data.

1. **linked / pointer**: arrays of pointers that point to other pointers and eventually the data. Dimensions can be non-uniform, data alignment can be achieved when allocating the innermost dimension.

Essentially, with method \#2 we differentiate between the memory required for pointers and the memory used for the actual data, which is located at the lowest dimension. Allocating the memory for the pointers usually involves individual allocation on every dimension recursively.

This also entails recursive (de-)allocation when copying or moving such a multi-dimensional data structure. For this reason, a **linear memory model** was chosen in this project, where both the data and all the pointers are stored linearly in a 'flat' array each.

This way, only 2 one-dimensional arrays (or similar data structure) need be allocated, regardless of the order of dimensions: one for the data, and one for the pointers.

### Self-Referencing Pointer Array
This array contains the entire pointer data required for a multi-dimensional buffer. The pointers are stored contiguously within every dimension, which allows for proper dereferencing / arithmetics when accessing the data through them. The pointers for each dimension point to the respective pointers in the next dimension, except for the 2nd-to-lowest dimension pointers, which point at the data.

Here's an example for a 5-Dimensional buffer with the extents `{2, 3, 2, 3, 6}`. The indices (0-55) represent the position of these pointers in pointer array.

![](PointerArrayGeometry.png)

## Memory Overhead & Buffer Geometry
The memory overhead of an N-Dimensional `HyperBuffer` of `base_type` has 2 components:

1. The amount of memory required for the **data** is given by the *product of all its dimension extents*. This corresponds to the amount of elements in the innermost dimension.

1. The **amount of pointers** need for a given multi-dimensional buffer is given by the *sum of the cumulative product of its dimension extents*, where with 'dimensions' we mean the 'pointer dimensions', i.e. we ignore the innermost dimension.

For a `HyperBuffer<float, 3>(2, 4, 5)` this would mean:

* `2*4*5 * sizeof(float) = 160 Bytes` for the data
* `(2 + 2*4) * sizeof(float*) = 80 Bytes` for the pointers (on a 64-bit machine)


# Lessons Learned: Unwanted Dynamic Memory (De-)Allocation

Since we could potentially use any data structure for both the pointers and data, `std::vector` is an obvious candidate. Defining a custom allocator would give us control over the allocation per se, but not over whether or when the allocator's `allocate()` function is called. As it turns out, the default constructor `std::vector` will allocate in some STL implementations, and not in others!

This means that we have to manually define copy and move constructors and assignment operators for our class and make sure the default constructor is NEVER called. This is not trivial.

The Microsoft Visual Studio Compiler (MSVC) allocates dynamic memory in the default constructors of some STL containers. This leads to unwanted allocation in move semantics, where the default constructor is implicitly called by the compiler at some point in the process. This has pushed us to avoid using STL containers in these scenarios.

 According to [https://stackoverflow.com/a/48744563/649700]() this happens when `_ITERATOR_DEBUG_LEVEL` is set to 1 or 2, which is the default setting for debug builds. Also cf. [https://docs.microsoft.com/en-us/cpp/standard-library/iterator-debug-level?view=vs-2019]()

To my knowledge, if you change `_ITERATOR_DEBUG_LEVEL` however, you have to change it globally within your build -- you cannot link items compiled with different `_ITERATOR_DEBUG_LEVEL` settings.
 


