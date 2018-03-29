#include <math.h>
#include <random>
#include <vector>
#include <fstream>
#include "force_directed.h"

// Helpers for force directed layout
std::pair<double, double> calculate_force(ForceDirectedParams& params, TNEANet& graph, TNEANet::TNodeI& node);
double distance_between(TNEANet& graph, int id1, int id2);
double distance_between(TNEANet::TNodeI& v1, TNEANet::TNodeI& v2);
std::vector<TNEANet::TEdgeI> incident_edges(int id, TNEANet& graph);

bool is_zero(double num) {
    if (abs(num) < 10) return true;
}

void random_layout(TNEANet& graph) {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 500.0);

    for (TNEANet::TNodeI node = graph.BegNI();
        node < graph.EndNI(); node++) {

        // Random coordinates
        graph.AddFltAttrDatN(node, distribution(generator), "x");
        graph.AddFltAttrDatN(node, distribution(generator), "y");
    }
}

double distance_between(TNEANet& graph, int id1, int id2) {
    return distance_between(graph.GetNI(id1), graph.GetNI(id2));
}

double distance_between(TNEANet::TNodeI& node1, TNEANet::TNodeI& node2) {
    return sqrt(
        pow(get_xy(node1).first - get_xy(node2).first, 2) + 
        pow(get_xy(node1).second - get_xy(node2).second, 2)
    );
}

std::pair<double, double> get_xy(TNEANet& graph, int id) {
    return get_xy(graph.GetNI(id));
}

std::pair<double, double> get_xy(TNEANet::TNodeI node) {
    TVec<TStr> attr_names;
    TVec<TFlt> attr_vals;
    node.GetFltAttrNames(attr_names);
    node.GetFltAttrVal(attr_vals);

    float x = attr_vals[attr_names.SearchForw("x")],
        y = attr_vals[attr_names.SearchForw("y")];

    return std::pair<double, double>(x, y);
}

std::pair<double, double> calculate_force(ForceDirectedParams& params, TNEANet& graph, TNEANet::TNodeI& node) {
    // Calculate the force on one node
    double sum_x = 0, sum_y = 0;
    double& luv = params.luv, kuv1 = params.kuv1, kuv2 = params.kuv2;

    // Iterate over incident edges
    for (auto edge : incident_edges(node.GetId(), graph)) {
        int dest_id = (edge.GetDstNId() != node.GetId()) ? edge.GetDstNId() : edge.GetSrcNId();
        if (dest_id != node.GetId()) { // Avoid loops
            sum_x += kuv1 * (distance_between(graph, dest_id, node.GetId()) - luv) *
                (get_xy(node).first - get_xy(graph, dest_id).first) /
                (distance_between(graph, dest_id, node.GetId()));

            sum_y += kuv1 * (distance_between(graph, dest_id, node.GetId()) - luv) *
                (get_xy(node).second - get_xy(graph, dest_id).second) /
                (distance_between(graph, dest_id, node.GetId()));
        }
    }

    // Iterate over vertices X vertices
    for (auto vertex = graph.BegNI(); vertex < graph.EndNI(); vertex++) {
        if (node.GetId() != vertex.GetId()) {
            sum_x += (kuv2 / pow(distance_between(vertex, node), 2)) *
                (get_xy(node).first - get_xy(vertex).first)/distance_between(vertex, node);

            sum_y += (kuv2 / pow(distance_between(vertex, node), 2)) *
                (get_xy(node).second - get_xy(vertex).second)/distance_between(vertex, node);
        }
    }

    return std::make_pair(sum_x, sum_y);
}

std::vector<TNEANet::TEdgeI> incident_edges(int id, TNEANet& graph) {
    std::vector<TNEANet::TEdgeI> edges;
    for (auto edge = graph.BegEI(); edge < graph.EndEI(); edge++)
        if (edge.GetDstNId() == id || edge.GetSrcNId() == id) edges.push_back(edge);

    return edges;
}

void force_directed_layout(ForceDirectedParams& params, TNEANet& graph) {
    // Layout points randomly
    random_layout(graph);
    std::map<TNEANet::TNodeI, std::pair<double, double>> forces;
    bool move = true;

    while (move) {
        for (auto node = graph.BegNI(); node < graph.EndNI(); node++) {
            auto coord = get_xy(node);
            auto force = calculate_force(params, graph, node);
            forces[node] = force;
        }

        // Keep moving as long as forces not zero
        move = false;
        for (auto force : forces) {
            std::cout << "Force: " << force.second.first << " " << 
                force.second.second << std::endl;
            if (!is_zero(sqrt(pow(force.second.first, 2) - pow(force.second.second, 2))))
                move = true;
        }

        std::cout << std::endl;

        // Move nodes...
        for (auto force : forces) {
            // ... in the direction of the force by a distance proportional to the magnitude of the force
            double pct = 1/sqrt(pow(force.second.first, 2) + pow(force.second.second, 2));
            graph.AddFltAttrDatN(force.first,
                get_xy(force.first).first - pct * force.second.first, "x");
            graph.AddFltAttrDatN(force.first,
                get_xy(force.first).second - pct * force.second.second, "y");
        }
        
        // Print out node locations
        for (auto node = graph.BegNI(); node < graph.EndNI(); node++)
            std::cout << get_xy(node).first << " " << get_xy(node).second << std::endl;

        std::cout << std::endl;
    }
}

SVG::SVG draw_graph(TNEANet& graph) {
    SVG::SVG root;
    auto edges = root.add_child<SVG::Group>(), vertices = root.add_child<SVG::Group>();
    edges->set_attr("stroke", "black").set_attr("stroke-width", "1px");

    // Map IDs to nodes
    std::map<int, SVG::Circle*> nodes;
    const float circle_radius = 10;

    for (auto node = graph.BegNI(); node < graph.EndNI(); node++) {
        auto coord = get_xy(node);
        nodes[node.GetId()] = vertices->add_child<SVG::Circle>(
            coord.first, coord.second, circle_radius);
    };

    for (auto edge = graph.BegEI(); edge < graph.EndEI(); edge++) {
        edges->add_child<SVG::Line>(
            nodes[edge.GetSrcNId()]->x(),
            nodes[edge.GetDstNId()]->x(),
            nodes[edge.GetSrcNId()]->y(),
            nodes[edge.GetDstNId()]->y()
        );
    }

    return root;
}   

int main() {
    /* Complete Graph
    TNEANet graph;
    int num_nodes = 10;
    for (int i = 0; i < num_nodes; i++)
        graph.AddNode(i);

    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++)
            if (i != j) graph.AddEdge(i, j);
    }
    */

    // Wheel
    TNEANet graph;
    int num_nodes = 6;
    for (int i = 0; i <= num_nodes; i++)
        graph.AddNode(i);

    // Perimeter of wheel
    for (int i = 0; i < num_nodes; i++) {
        if (i + 1 == num_nodes) {
            graph.AddEdge(i, 0);
            std::cout << "Added an edge from " << i << " to " << 0 << std::endl;
        }
        else {
            graph.AddEdge(i, i + 1);
            std::cout << "Added an edge from " << i << " to " << i + 1<< std::endl;
        }
    }

    // Spokes
    for (int i = 0; i < num_nodes; i++)
        graph.AddEdge(i, num_nodes);

    // random_layout(graph);
    
    // Note algorithm converges more quickly if
    // natural spring length is reasonably large
    ForceDirectedParams params = { 400, 40, 40 };
    force_directed_layout(params, graph);

    std::ofstream graph_out("graph.svg");
    auto force_dir_draw = draw_graph(graph);
    force_dir_draw.autoscale();

    graph_out << std::string(force_dir_draw);
    return 0;
}