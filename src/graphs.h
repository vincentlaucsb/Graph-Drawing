#pragma once
/** Functions for creating graphs */
#include "force_directed.h"

TNEANet cycle(int nodes);
TNEANet complete_bipartite(int m, int n);
TNEANet complete(int nodes);
TNEANet prism(int n);
TNEANet wheel(int n);
TNEANet ladder(int);
TNEANet petersen();
TNEANet hypercube();