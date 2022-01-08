```
 ╦ ╦┬ ┬┌─┐┌─┐┬─┐  ╔╗ ┬ ┬┌─┐┌─┐┌─┐┬─┐
 ╠═╣└┬┘├─┘├┤ ├┬┘  ╠╩╗│ │├┤ ├┤ ├┤ ├┬┘
 ╩ ╩ ┴ ┴  └─┘┴└─  ╚═╝└─┘└  └  └─┘┴└─

```

### A C++ structure to manage multi-dimensional data efficiently and safely

![](https://img.shields.io/github/license/Sidelobe/Hyperbuffer)
![](https://img.shields.io/badge/C++14-header--only-blue.svg?style=flat&logo=c%2B%2B)
![](https://img.shields.io/badge/dependencies-STL_only-blue)

This container was designed to hold dynamically-allocated N-dimensional datasets in memory and provide convenient access to it, while minimizing performance/memory overhead as well as using a single allocation for all the data.

### Usage Example
```cpp
HyperBuffer<float, 2> buffer2D (2, 5);

float element01 = buffer2D[0][1];
float element14 = buffer2D.at(1, 4); // alternative to []
float** rawPointer = buffer2D.data();
float* outerDimension0 = buffer2D[0];

HyperBufferView<float, 1> subView = buffer2D.subView(1);
float* outerDimension1 = subView.data();

// Any number of dimensions
HyperBuffer<int, 8> buffer8D (3, 4, 3, 1, 6, 256, 11, 7); 

// Wrapper for existing multi-dimensional data (zero dynamic memory allocation!)
float bufferL[]{ 0.1f, 0.2f, 0.3f };  float bufferR[]{ -0.1f, -0.2f, -0.3f };
float* stereoBuffer[2] = { bufferL, bufferR };
HyperBufferViewNC<float, 2> wrapper(stereoBuffer, 2, 3);
wrapper[0][1] = 0.22f; // access left channel, second sample
```

### Requirements / Compatibility

 - C++14, STL only
 - Compiled & Tested with:
 	- GCC/g++ 9.3.0 (Linux)
	- Clang 11 (Xcode 11.6, macos)
	- MSVC++ 19.28 (Visual Studio 2019, Windows) 

## Design paradigms:

HyperBuffer is designed as a multi-dimensional counterpart to `std::array` and/or `std::vector`. However, it differs from said standard library classes on the 'points of commitment', i.e. the point in time at which certain parameters have to be specified (and cannot be changed afterwards):

|                          | `HyperBuffer`           | `std::array` | `std::vector` |
|--------------------------|:-----------------------:|:------------:|:------------:|
| element data type (T)    | compile-time            | compile-time | compile-time |
| number of dimensions (N) | compile-time            | = 1          | = 1          |
| extent of dimensions     | construction-time       | compile-time | run-time     |

`HyperBuffer` is thus a non-resizable container like `std::array`, however in contrast, the extent of the dimensions **can** be specified at runtime.

>**Note**: For the time being, dimensions are constrained to be uniform, i.e. each 'slice' in a given dimension has equal length and data type.

Design choices were carefully weighed with the following prime directive in mind: avoid dynamic memory allocation as much as possible. This is crucial in realtime environments with a strict need for deterministic behaviour (e.g. audio processing threads). 

Thanks to the chosen memory model, **dynamic memory allocation happens only during construction**. Furthermore, the entire data and pointer memory is each **allocated in a single call** (cf. documentation in [Design Details](docu/HyperBuffer%20Design%20Details.md)), thereby avoiding memory fragmentation / churn.

### Data Storage & Ownership Variants

`HyperBuffer` comes in 3 incarnations that use different levels of ownership on the data. In multi-dimensional structures, we can differentiate between the memory required to store the pointers.

|                     | ownership                                | use case                                                                                              |
|---------------------|------------------------------------------|-------------------------------------------------------------------------------------------------------|
| `HyperBuffer`       | owns/allocates pointers & data                     | Storing multi-dimensional data and providing a simple and safe API to it.                                                                                                      |
| `HyperBufferView`   | owns pointers, externally-allocated data | View for existing data in the HyperBuffer memory format (contiguous 1D memory) - e.g. a view to a sub-dimension of `HyperBuffer`                                                                          |
| `HyperBufferViewNC` | externally-allocated pointers & data | Wrapper for existing multi-dimensional data (non-contiguous memory, e.g. `float**`); gives it the same API as `HyperBuffer` |

>**Note**: Behaviour on copy & move: `HyperBuffer` copies/moves the data like a normal object with data ownership. When copying `HyperBufferViewNC ` and `HyperBufferView`, however, the data is not duplicated - the copy references the original data as well.

### API features & Memory Management:

In addition to information about the geometry (dimensions and extent thereof), the API has several ways of accessing data:

| function    | description   | return value       | `HyperBuffer`  | `HyperBufferView` | `HyperBufferViewNC` |
|-------------|---------------|--------------------|----------------|:---------------:|:--------------:|
| `.data()` | access the start of highest dimension of the data | raw pointer (e.g. `float***`) | non-allocating | non-allocating  | non-allocating |
| `operator[.]` | access the N-1 sub-dimension at the given index; can be chained: `h[3][0][6]` | raw pointer  (e.g. `float**`); data value if `N==1` | non-allocating | non-allocating  | non-allocating |
| `at(...)` | access data in lowest dimension (N arguments) | data value (e.g. `float`) | **allocating** | **allocating**  | non-allocating |
| `subView(...)` | access data in any dimension (variable-length argument) | N-x view to the data | **allocating** | **allocating**  | non-allocating |

While `HyperBufferViewNC` never allocates memory under any circumstances, you can see above that the `.at()` and `subView()`accessors allocate dynamic memory for the other variants. This is because a new N-1 `HyperBufferView` is constructed, which allocates memory for the pointers.

Further guarantees:

* accessing data is always allocation-free
* dynamic allocation-free move() semantics
* (*planned*) alignment of the data (lowest-order/innermost dimension) can be specified ('owning' mode only)
 
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
