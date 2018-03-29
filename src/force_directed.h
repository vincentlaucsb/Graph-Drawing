#pragma once
#include "svg.hpp"
#include "Snap.h"

struct ForceDirectedParams {
    double luv;
    double kuv1;
    double kuv2;
};

bool is_zero(double);
std::pair<double, double> get_xy(TNEANet& graph, int id);
std::pair<double, double> get_xy(TNEANet::TNodeI node);
SVG::SVG draw_graph(TNEANet& graph);
void random_layout(TNEANet& graph);
void force_directed_layout(ForceDirectedParams& params, TNEANet& graph);