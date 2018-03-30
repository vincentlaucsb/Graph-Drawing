#include <math.h>
#include <random>
#include <vector>
#include <set>
#include <fstream>
#include "graphs.h"

// Helpers for spring force directed layout
Point calculate_force(ForceDirectedParams& params, TNEANet& graph, TNEANet::TNodeI& node,
    std::map<int, VertexSet>& adjacent);
double distance_between(TNEANet& graph, int id1, int id2);
double distance_between(TNEANet::TNodeI& v1, TNEANet::TNodeI& v2);

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

inline double distance_between(TNEANet& graph, int id1, int id2) {
    return distance_between(graph.GetNI(id1), graph.GetNI(id2));
}

inline double distance_between(TNEANet::TNodeI& node1, TNEANet::TNodeI& node2) {
    return sqrt(
        pow(get_xy(node1).first - get_xy(node2).first, 2) + 
        pow(get_xy(node1).second - get_xy(node2).second, 2)
    );
}

inline Point get_xy(TNEANet& graph, int id) {
    return get_xy(graph.GetNI(id));
}

inline Point get_xy(TNEANet::TNodeI node) {
    TVec<TStr> attr_names;
    TVec<TFlt> attr_vals;
    node.GetFltAttrNames(attr_names);
    node.GetFltAttrVal(attr_vals);

    float x = attr_vals[attr_names.SearchForw("x")],
        y = attr_vals[attr_names.SearchForw("y")];

    return Point(x, y);
}

inline Point calculate_force(ForceDirectedParams& params, TNEANet& graph, TNEANet::TNodeI& node,
    std::map<int, VertexSet>& adjacent) {
    // Calculate the force on one node
    double sum_x = 0, sum_y = 0;
    double& luv = params.luv, kuv1 = params.kuv1, kuv2 = params.kuv2;

    // Iterate over adjacent vertices
    for (auto adj: adjacent[node.GetId()]) {
        sum_x += kuv1 * (distance_between(graph, adj, node.GetId()) - luv) *
            (get_xy(node).first - get_xy(graph, adj).first) /
            (distance_between(graph, adj, node.GetId()));

        sum_y += kuv1 * (distance_between(graph, adj, node.GetId()) - luv) *
            (get_xy(node).second - get_xy(graph, adj).second) /
            (distance_between(graph, adj, node.GetId()));
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

EdgeSet incident_edges(int id, const TNEANet& graph) {
    /** Compute all edges incident to a vertex */
    EdgeSet edges;
    for (auto edge = graph.BegEI(); edge < graph.EndEI(); edge++)
        if (edge.GetDstNId() == id || edge.GetSrcNId() == id) edges.insert(edge);
    return edges;
}

VertexSet adjacent_vertices(int id, const TNEANet& graph) {
    /** Compute all vertices adjacent to vertex with given id by iterating
     *  through edge set
     */
    VertexSet ret;
    auto edges = incident_edges(id, graph);
    for (auto edge : edges) {
        if (edge.GetDstNId() == id) ret.insert(edge.GetSrcNId());
        else if (edge.GetSrcNId() == id) ret.insert(edge.GetDstNId());
    }
    return ret;
}

std::map<int, VertexSet> adjacency_list(const TNEANet& graph) {
    /** Compute a graph's adjacency list */
    std::map<int, VertexSet> ret;
    for (auto node = graph.BegNI(); node < graph.EndNI(); node++)
        ret[node.GetId()] = adjacent_vertices(node.GetId(), graph);
    return ret;
}

void force_directed_layout(ForceDirectedParams& params, TNEANet& graph) {
    // Layout points randomly
    random_layout(graph);
    std::map<TNEANet::TNodeI, Point> forces;
    std::map<int, VertexSet> adjacent = adjacency_list(graph); // Optimization

    bool move = true;
    while (move) {
        for (auto node = graph.BegNI(); node < graph.EndNI(); node++) {
            auto coord = get_xy(node);
            auto force = calculate_force(params, graph, node, adjacent);
            forces[node] = force;
        }

        // Keep moving as long as forces not zero
        move = false;
        for (auto force : forces) {
            if (!APPROX_EQUALS(sqrt(pow(force.second.first, 2) - pow(force.second.second, 2)), 0, 5))
                move = true;
        }

        // Move nodes...
        for (auto force : forces) {
            // ... in the direction of the force by a distance proportional to the magnitude of the force
            //double pct = 1/sqrt(pow(force.second.first, 2) + pow(force.second.second, 2));
            double pct = 0.1;
            if (isnan(force.second.first)) throw std::runtime_error("Failed to converge");
            graph.AddFltAttrDatN(force.first,
                get_xy(force.first).first - pct * force.second.first, "x");
            graph.AddFltAttrDatN(force.first,
                get_xy(force.first).second - pct * force.second.second, "y");
        }
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
        nodes[node.GetId()] = vertices->add_child<SVG::Circle>(coord, circle_radius);
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

// Helpers for Barycenter Draw
void barycenter_layout(TNEANet& graph, const size_t fixed_vertices) {
    using namespace SVG;
    std::set<TNEANet::TNodeI> fixed, free;
    std::vector<Point> polygon = util::polar_points((int)fixed_vertices, 0, 0, 250);

    auto node = graph.BegNI();
    auto point = polygon.begin();
    for (; (node < graph.EndNI()) && (fixed.size() < fixed_vertices); node++) {
        fixed.insert(node);

        // Place along polygon
        graph.AddFltAttrDatN(node, point->first, "x");
        graph.AddFltAttrDatN(node, point->second, "y");
        point++;
    }

    for (; node < graph.EndNI(); node++) {
        free.insert(node);

        // Place free vertices at origin
        graph.AddFltAttrDatN(node, 0, "x");
        graph.AddFltAttrDatN(node, 0, "y");
    }

    // Optimization: Keep a list of adjacent vertices
    std::map<int, VertexSet> adjacent = adjacency_list(graph);

    bool converge;
    do {
        converge = true;
        for (auto node: free) {
            auto current_xy = get_xy(node);
            double sum_x = 0, sum_y = 0, new_x, new_y;

            // Sum up adjacent vertices
            for (auto u : adjacent[node.GetId()]) {
                auto u_xy = get_xy(graph.GetNI(u));
                sum_x += u_xy.first;
                sum_y += u_xy.second;
                // std::cout << "u_xy.first " << u_xy.first << " u_xy.second " << u_xy.second << std::endl;
            }

            new_x = (1 / (double)node.GetDeg()) * sum_x;
            new_y = (1 / (double)node.GetDeg()) * sum_y;
            graph.AddFltAttrDatN(node, new_x, "x");
            graph.AddFltAttrDatN(node, new_y, "y");

            // Convergence test
            if (!(APPROX_EQUALS(new_x, current_xy.first, 0.01) && APPROX_EQUALS(new_y, current_xy.second, 0.01)))
                converge = false;
        }
    } while (!converge);
}

int main() {
    // Complete Graph (K5, K7, K8 is good with springs)
    // auto graph = complete(20);

    // auto graph = petersen();

    // Cycle
    // auto graph = cycle(40);

    auto pris_g = prism(50);
    auto pgraph = petersen();

    // Good complete graph settings
    // ForceDirectedParams params = { 400, 2, 1 };
    
    // Note algorithm converges more quickly if
    // natural spring length is reasonably large
    try {
        auto& graph = pris_g;
        ForceDirectedParams params = { 200, 2, 1 };
        force_directed_layout(params, graph);
        auto force_dir_draw = draw_graph(graph);
        force_dir_draw.autoscale();
        std::ofstream graph_out("graph.svg");
        graph_out << std::string(force_dir_draw);
    }
    catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }
    
    // barycenter_layout(kmn, 3);
    barycenter_layout(pgraph, 5);

    std::ofstream petersen_out("petersen.svg");
    auto petersen_draw = draw_graph(pgraph);
    petersen_draw.autoscale();

    petersen_out << std::string(petersen_draw);
    return 0;
}