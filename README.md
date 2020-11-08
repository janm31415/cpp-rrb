# cpp-rrb
C++ port of Jean Niklas L'orange's RRB-tree implementation

Relaxed Radix Balanced trees (RRB-trees) are immutable data structures with good performance
for all vector-like operations, including insertion, erasion, and concatenation.

This implementation is a C++ port of the library [c-rrb](https://github.com/hypirion/c-rrb), with the
following modifications:

- Garbage collection (Boehm GC) has been replaced by reference counting.
- The original c-rrb implementation stores objects in the rrb-tree as boxed objects (i.e. pointers).
  Here we store them unboxed (as values sequentially in memory per tree node).
  
The main data structure has three template parameters:
```
  template <typename T, bool atomic_ref_counting, int N>
  struct rrb;  
```
The first parameter `T` is the type of the elements.
The second parameter `bool atomic_ref_counting` indicates whether the reference counting should be 
thread-safe or not. If `false` and thus not thread-safe, the performance is faster.
The third parameter `int N` indicates the branching factor of the tree. The default is 5, which 
corresponds to a branching factor of 2^5.

The usage of the rbb tree has been wrapped in a more familiar vector-like structure:
```
  template <typename T, bool atomic_ref_counting, int N>
  class vector;
```
similar to `immer::flex_vector` of the library [immer](https://github.com/arximboldi/immer).

This library has been tested on Windows 10 using Visual Studio 2017/2019, on Ubuntu 18.04.4 with gcc 7.5.0, and on MacOS 10.15.6 with XCode 11.7. You best use CMake to generate a solution file or makefile or XCode project.

For a practical application, see my [jed](https://github.com/janm31415/jed) project: a minimalist text editor inspired by [Acme](http://acme.cat-v.org/) and [Nano](https://github.com/madnight/nano).
