#include "force_directed.h"
#include "cxxopts.hpp"

int main(int argc, char** argv) {
    using namespace force_directed;
    prism_distances(3, 40);
    return 0;
}
