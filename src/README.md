# A Short Guide to the C++ Source Code
Unfortunately, as a lot of code written for academic purposes goes, the quality tends to take a nose dive as deadlines loom. However, most of the code here is relatively well-structured and commented, especially the algorithms that actually ended up appearing in my paper. :)

File             | Description
---------------- | -------------
force_directed.h | Header file for all force directed algorithms (Eades, Tutte)
layout.cpp       | The implementation for layout algorithms
graphs.cpp       | Some common graphs (not all were used in the paper--those that weren't in the paper aren't guaranteed to be implemented correctly)
tree_lp.h        | Header file for Supowit-Reingold Algorithm which also defines a tree data structure
tree_lp.cpp      | Implementation of Supowit-Reingold Algorithm
bst.hpp          | Implementation of a basic binary search tree

## External Libraries
As seen under ![https://github.com/vincentlaucsb/Graph-Drawing/tree/master/lib](../lib/), many external libraries created by myself and others were used. These were:

 * csv-parser: A CSV parser I wrote
 * svg: A library for generating scalable vector graphics I wrote
 * cxxopts: A C++ library for parsing command line arguments
 * snap: The Stanford Network Analysis Project which provides high performance graph data structures
 * Eigen: A high performance C++ linear algebra library
 * glpk: The GNU Linear Programming Kit which solves linear programs with the simplex method