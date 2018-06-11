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

    std::pair<double, double> get_xy(TNEANet& graph, int id);
    std::pair<double, double> get_xy(TNEANet::TNodeI node);
    SVG::SVG draw_graph(TNEANet& graph, const double width = 500);
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
    
    std::vector<SVG::SVG> barycenter_layout(TNEANet& graph,
        const size_t fixed_vertices = 5, const double width = 500);
    BarycenterLayout barycenter_layout_la(TNEANet& graph, const size_t fixed_vertices, const double width = 500);

    // Helpers
    using EdgeSet = std::set<TNEANet::TEdgeI>;
    using VertexSet = std::set<int>;

    AdjacencyList adjacency_list(TUNGraph& graph);
    EdgeSet incident_edges(int id, const TNEANet& graph);
    VertexSet adjacent_vertices(int id, const TNEANet& graph);
    std::map<int, VertexSet> adjacency_list(const TNEANet& graph);

    // Functions for creating graphs
    TNEANet cycle(int nodes);
    TNEANet complete_bipartite(int m, int n);
    TNEANet complete(int nodes);
    TNEANet prism(int n);
    TUNGraph wheel_un(int n);
    TUNGraph hypercube();
    TNEANet wheel(int n);
    TNEANet ladder(int);
    TNEANet petersen();
    TUNGraph hypercube_4();
    TUNGraph tree(int height);
    TUNGraph three_reg_6();
}