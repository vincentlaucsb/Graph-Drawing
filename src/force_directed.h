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
    using AdjacencyList = std::map<int, std::set<int>>;
    using VertexPos = std::map<int, SVG::Point>;
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

    std::pair<double, double> get_xy(TUNGraph& graph, int id);
    std::pair<double, double> get_xy(TUNGraph::TNodeI node);
    SVG::SVG draw_graph(TUNGraph& graph, VertexPos& pos, const double width = 500);
    
    VertexPos random_layout(TUNGraph&);
    std::vector<SVG::SVG> eades84(TUNGraph& graph);
    std::vector<SVG::SVG> eades84(TUNGraph& graph, VertexPos& pos);
    std::vector<SVG::SVG> eades84_2(ForceDirectedParams& params, TUNGraph& graph);
    std::vector<SVG::SVG> eades84_2(ForceDirectedParams& params, TUNGraph& graph, VertexPos& pos);

    namespace eades84_helper {
        double distance_between(VertexPos& pos, int node1, int node2);
        Point calculate_force(ForceDirectedParams& params, TUNGraph& graph, int node,
            AdjacencyList& adjacent, VertexPos& pos);
    }
    
    std::vector<SVG::SVG> barycenter_layout(TUNGraph& graph,
        const size_t fixed_vertices = 5, const double width = 500);
    BarycenterLayout barycenter_layout_la(TUNGraph& graph,
        const size_t fixed_vertices, const double width = 500);

    // Helpers
    using EdgeSet = std::set<TUNGraph::TEdgeI>;
    using VertexSet = std::set<int>;

    AdjacencyList adjacency_list(TUNGraph& graph);
    EdgeSet incident_edges(int id, const TUNGraph& graph);
    VertexSet adjacent_vertices(int id, const TUNGraph& graph);
    std::map<int, VertexSet> adjacency_list(const TUNGraph& graph);

    // Functions for creating graphs
    TUNGraph cycle(int nodes);
    TUNGraph complete_bipartite(int m, int n);
    TUNGraph complete(int nodes);
    TUNGraph prism(int n);
    TUNGraph hypercube();
    TUNGraph wheel(int n);
    TUNGraph ladder(int);
    TUNGraph petersen();
    TUNGraph hypercube_4();
    TUNGraph tree(int height);
    TUNGraph three_reg_6();
}