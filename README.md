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


### Inspiration
A lot of inspiration came from this excellent article, which follows a similar philosophy in many aspects:<br>
[https://www.codeproject.com/Articles/5250544/Implementation-of-Resizable-Multidimensional-Array]()