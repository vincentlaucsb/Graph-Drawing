#pragma once
#define NOMINMAX // Thanks Windows.h
#include "../lib/Eigen/Dense"
#include "Snap.h"
#include "svg.hpp"
#include <math.h>
#include <random>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <set>

namespace force_directed {
    using SVG::Point;
    using Eigen::MatrixXd;
    using Eigen::VectorXd;

    struct ForceDirectedParams {
        double luv;
        double kuv1;
        double kuv2;
    };

    struct BarycenterLayout {
        /** Return value of linear algebra based solver */
        SVG::SVG image;
        MatrixXd matrix;
        VectorXd fixed_x;
        VectorXd fixed_y;
        VectorXd sol_x;
        VectorXd sol_y;
    };

    std::pair<double, double> get_xy(TNEANet& graph, int id);
    std::pair<double, double> get_xy(TNEANet::TNodeI node);
    SVG::SVG draw_graph(TNEANet& graph, const double width = 500);
    void random_layout(TNEANet& graph);
    std::vector<SVG::SVG> force_directed_layout(ForceDirectedParams& params, TNEANet& graph);
    void force_directed_layout_la(ForceDirectedParams& params, TNEANet& graph);
    std::vector<SVG::SVG> barycenter_layout(TNEANet& graph,
        const size_t fixed_vertices = 5, const double width = 500);
    BarycenterLayout barycenter_layout_la(TNEANet& graph, const size_t fixed_vertices, const double width = 500);

    // Helpers
    using EdgeSet = std::set<TNEANet::TEdgeI>;
    using VertexSet = std::set<int>;

    EdgeSet incident_edges(int id, const TNEANet& graph);
    VertexSet adjacent_vertices(int id, const TNEANet& graph);
    std::map<int, VertexSet> adjacency_list(const TNEANet& graph);
    void prism_distances(int min = 3, int max = 10);

    // Functions for creating graphs
    TNEANet cycle(int nodes);
    TNEANet complete_bipartite(int m, int n);
    TNEANet complete(int nodes);
    TNEANet prism(int n);
    TNEANet wheel(int n);
    TNEANet ladder(int);
    TNEANet petersen();
    TNEANet hypercube();
    TNEANet hypercube_4();
    TNEANet tree();
}