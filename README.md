# Dots and Lines: A Survey of Graph Drawing Algorithms
This repository contains the report and code for my senior thesis paper on graph drawing algorithms. In mathematics, graphs are a collection of nodes connected to each other by vertices. More concretely, a social network (e.g. Facebook) can be thought of a graph if nodes represent people and vertices represent friendships. Because of the importance of graphs in many fields outside math, including computer science, algorithms for the visualization of graphs have been widely studied. In my paper, I give extra attention to three selected algorithms.

 * [Read the report](https://github.com/vincentlaucsb/Graph-Drawing/blob/master/report.pdf) for a detailed explication of the algorithms, and some discussion of the mathematics (including proofs)
 * For some cool animations of force directed algorithms at work, [click here](https://github.com/vincentlaucsb/Graph-Drawing/tree/master/animations)
 * For the C++ code behind these images, [click here](https://github.com/vincentlaucsb/Graph-Drawing/tree/master/src)
 
 And of course, for my original inspiration [read this book](https://www.amazon.com/Graph-Drawing-Algorithms-Visualization-Graphs/dp/0133016153).
 
## C++ Information
This project was built as a series of C++11 programs pusing Microsoft Visual Studio v15.6.4 on Windows 10 using CMake. There are three main programs:

 * animate_spring.exe: A program for creating animations and static drawings of Eades' algorithm
 * animate_tutte.exe: A program for creating animations of Tutte's algorithm or static drawings using a linear algebra based solver
 * tree_lp.exe: A program for drawing (random) binary trees via a linear program

## Supowit and Reingold's Tree Drawing Program
In their 1983 paper on the complexity of tree drawing, Supowit and Reingold describe a linear program for drawing binary trees. A result of this algorithm is shown below:

<img width="800px" src="https://raw.githubusercontent.com/vincentlaucsb/Graph-Drawing/master/report/bst_large.svg?sanitize=true" />

## Force Directed Algorithms
### Eades' Spring Force Directed Algorithm
This algorithm visualizes nodes as rings hooked onto steel springs (edges). In addition, there's also an electric force between vertices, so that they don't completely overlap.

### Tesseract
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Graph-Drawing/master/report/tesseract.svg?sanitize=true" />

### Algorithm Trace of Eades' Algorithm for K9
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/animations/k9.svg?sanitize=true" />

## Tuttes' Barycenter Algorithm
In this classic force directed algorithm, a strictly convex polygon is drawn on the exterior, with some nodes nailed down onto the
vertices on this polygon. Then, the other nodes are placed in the "center of mass" of this polygon. This is an algorithm of limited utility, since it doesn't handle many nodes, and only gives aesthetically pleasing
results for 3-connected graphs. However, some of the results of this algorithm are pretty slick.

### Durer Graph
<img width="500px" src="https://github.com/vincentlaucsb/Graph-Drawing/blob/master/report/barycenter/durer.svg?sanitize=true" />

### Cubic Symmetric Graph F<sub>048</sub>A
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Graph-Drawing/master/report/barycenter/cubic_symmetric_f048_a.svg?sanitize=true" />
