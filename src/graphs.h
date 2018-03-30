#pragma once
/** Functions for creating graphs */
#include "force_directed.h"

TNEANet cycle(int nodes);
TNEANet complete(int nodes);
TNEANet wheel(int n);
TNEANet petersen();
TNEANet hypercube();