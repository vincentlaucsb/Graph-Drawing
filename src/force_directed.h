#pragma once
#include "svg.hpp"
#include "Snap.h"
#include <set>

using SVG::Point;

struct ForceDirectedParams {
    double luv;
    double kuv1;
    double kuv2;
};

std::pair<double, double> get_xy(TNEANet& graph, int id);
std::pair<double, double> get_xy(TNEANet::TNodeI node);
SVG::SVG draw_graph(TNEANet& graph);
void random_layout(TNEANet& graph);
void force_directed_layout(ForceDirectedParams& params, TNEANet& graph);
void barycenter_layout(TNEANet& graph, const size_t fixed_vertices = 5);

// Helpers
using EdgeSet = std::set<TNEANet::TEdgeI>;
using VertexSet = std::set<int>;

EdgeSet incident_edges(int id, const TNEANet& graph);
VertexSet adjacent_vertices(int id, const TNEANet& graph);
std::map<int, VertexSet> adjacency_list(const TNEANet& graph);