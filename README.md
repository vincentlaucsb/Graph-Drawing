# Force-Directed-Drawing
A demonstration of force-directed graph drawing algorithms. Currently, there are implementations of Eades'
spring algorithm and Tutte's barycenter drawing algorithm. While force-directed algorithms vary widely in their
their methods, efficiency, scale, and utility, they are all based on some physical analogy.

## Eades' Spring Force Directed Algorithm
This algorithm visualizes nodes as rings hooked onto steel springs (edges). In addition, there's also an electric force between
vertices, so that they don't completely overlap.

### Algorithm Trace of Eades' Algorithm for K9
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/k9.svg?sanitize=true" />

### Other Complete Graphs
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/k4.svg?sanitize=true" />
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/k5.svg?sanitize=true" />

## Tuttes' Barycenter Algorithm
In this classic force directed algorithm, a strictly convex polygon is drawn on the exterior, with some nodes nailed down onto the
vertices on this polygon. Then, the other nodes are placed in the "center of mass" of this polygon.
This is an algorithm of limited utility, since it doesn't handle many nodes, and only gives aesthetically pleasing
results for 3-connected graphs.

### Hypercube
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/hypercube.svg?sanitize=true" />

### Petersen
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/petersen.svg?sanitize=true" />

### Prisms
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/prism_5.svg?sanitize=true" />
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/prism_8.svg?sanitize=true" />
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/prism_10.svg?sanitize=true" />
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/prism_15.svg?sanitize=true" />

### Angular Resolution
One of the drawbacks of this algorithm is that the angular resolution becomes poorer as the number of vertices increases. This 
can be seen in the prisms on 20 and 40 vertices respectively shown below.

<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/prism_20.svg?sanitize=true" />
<img width="500px" src="https://raw.githubusercontent.com/vincentlaucsb/Force-Directed-Drawing/master/prism_40.svg?sanitize=true" />
