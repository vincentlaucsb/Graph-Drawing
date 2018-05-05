#include "force_directed.h"

namespace force_directed {
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
        for (auto adj : adjacent[node.GetId()]) {
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
                    (get_xy(node).first - get_xy(vertex).first) / distance_between(vertex, node);

                sum_y += (kuv2 / pow(distance_between(vertex, node), 2)) *
                    (get_xy(node).second - get_xy(vertex).second) / distance_between(vertex, node);
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

    std::vector<SVG::SVG> force_directed_layout(ForceDirectedParams& params, TNEANet& graph) {
        // Use Eades' spring layout algorithm, creating a frame between each iteration
        std::vector<SVG::SVG> ret;

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
                double pct = 0.1;
                if (isnan(force.second.first)) throw std::runtime_error("Failed to converge");
                graph.AddFltAttrDatN(force.first,
                    get_xy(force.first).first - pct * force.second.first, "x");
                graph.AddFltAttrDatN(force.first,
                    get_xy(force.first).second - pct * force.second.second, "y");
            }

            // Add frame
            ret.push_back(draw_graph(graph));
        }

        return ret;
    }

    SVG::SVG draw_graph(TNEANet& graph, const double width) {
        SVG::SVG root;
        auto edges = root.add_child<SVG::Group>(), vertices = root.add_child<SVG::Group>();
        edges->set_attr("stroke", "black").set_attr("stroke-width", "1px");

        // Map IDs to nodes
        std::map<int, SVG::Circle*> nodes;
        const double circle_radius = std::max(5.0, width / 50);

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

    std::vector<SVG::SVG> barycenter_layout(TNEANet& graph, const size_t fixed_vertices, const double width) {
        std::vector<SVG::SVG> ret;
        std::set<TNEANet::TNodeI> fixed, free;
        std::vector<Point> polygon = SVG::util::polar_points((int)fixed_vertices, 0, 0, width/2);

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

        // Draw initial positions
        ret.push_back(draw_graph(graph));

        // Optimization: Keep a list of adjacent vertices
        std::map<int, VertexSet> adjacent = adjacency_list(graph);

        bool converge;
        do {
            converge = true;
            for (auto node : free) {
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

            // Algorithm trace
            ret.push_back(draw_graph(graph, width));
        } while (!converge);

        return ret;
    }

    BarycenterLayout barycenter_layout_la(
        TNEANet& graph, const size_t fixed_vertices, const double width) {
        /** Solve the barycenter layout problem using linear algebra */
        std::vector<TNEANet::TNodeI> fixed, free;
        std::vector<Point> polygon = SVG::util::polar_points((int)fixed_vertices, 0, 0, width / 2);

        auto node = graph.BegNI();
        auto point = polygon.begin();
        for (; (node < graph.EndNI()) && (fixed.size() < fixed_vertices); node++) {
            fixed.push_back(node);

            // Place along polygon
            graph.AddFltAttrDatN(node, point->first, "x");
            graph.AddFltAttrDatN(node, point->second, "y");
            point++;
        }

        for (; node < graph.EndNI(); node++) free.push_back(node);

        MatrixXd points(free.size(), free.size());
        VectorXd x(free.size()), y(free.size());;

        // Populate the matrix
        for (int i = 0; i < (int)free.size(); i++) {
            for (int j = 0; j < (int)free.size(); j++) {
                if (i == j) points(i, j) = (int)free[i].GetDeg();
                else if (free[i].IsNbrNId(free[j].GetId())) points(i, j) = -1;
                else points(i, j) = 0;
            }
        }

        // Populate vectors of fixed positions
        for (int i = 0; i < fixed.size(); i++) {
            x(i) = get_xy(fixed[i]).first;
            y(i) = get_xy(fixed[i]).second;
        }

        // std::cout << "Here is the matrix A:\n" << points << std::endl;
        // std::cout << "Here is the vector b:\n" << vars << std::endl;
        VectorXd sol_x = points.fullPivHouseholderQr().solve(x),
            sol_y = points.fullPivHouseholderQr().solve(y);
        // std::cout << "The solution is:\n" << sol << std::endl;

        // Set graph positions
        for (size_t i = 0; i < sol_x.size(); i++) {
            graph.AddFltAttrDatN(free[i], sol_x(i), "x");
            graph.AddFltAttrDatN(free[i], sol_y(i), "y");
        }

        return { draw_graph(graph), points, x, y, sol_x, sol_y };
    }
}