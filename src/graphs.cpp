#include "force_directed.h"

namespace force_directed {
    TNEANet cycle(int nodes) {
        TNEANet graph;
        for (int i = 0; i < nodes; i++)
            graph.AddNode(i);

        for (int i = 0; i + 1 < nodes; i++) {
            graph.AddEdge(i, i + 1);
        }

        graph.AddEdge(nodes - 1, 0);
        return graph;
    }

    TNEANet ladder(int rungs) {
        // Ref: http://mathworld.wolfram.com/LadderGraph.html
        TNEANet graph;
        graph.AddNode(0);
        graph.AddNode(1);
        graph.AddEdge(0, 1);

        for (int i = 1; i < rungs; i++) {
            graph.AddNode(2 * i);
            graph.AddNode((2 * i) + 1);
            graph.AddEdge(2 * i, (2 * i) + 1);

            // Connect to previous rung
            graph.AddEdge(2 * i, (2 * i) - 2);
            graph.AddEdge((2 * i) + 1, (2 * i) - 1);
        }

        return graph;
    }

    TNEANet complete(int nodes) {
        TNEANet graph;
        for (int i = 0; i < nodes; i++)
            graph.AddNode(i);

        for (int i = 0; i < nodes; i++) {
            for (int j = 0; j < nodes; j++)
                if (i != j) graph.AddEdge(i, j);
        }

        return graph;
    }

    TNEANet complete_bipartite(int m, int n) {
        TNEANet graph;
        for (int i = 0; i < m + n; i++) graph.AddNode(i);

        for (int i = 0; i < m; i++) { // Iterate over left side
            // Add edges to right
            for (int j = m; j < m + n; j++) {
                graph.AddEdge(i, j);
            }
        }

        return graph;
    }

    // Petersen
    TNEANet petersen() {
        TNEANet graph;
        for (int i = 0; i < 10; i++)
            graph.AddNode(i);

        // Perimeter
        graph.AddEdge(0, 1);
        graph.AddEdge(1, 2);
        graph.AddEdge(2, 3);
        graph.AddEdge(3, 4);
        graph.AddEdge(4, 0);

        // Spokes
        graph.AddEdge(0, 5);
        graph.AddEdge(1, 6);
        graph.AddEdge(2, 7);
        graph.AddEdge(3, 8);
        graph.AddEdge(4, 9);

        // Star
        graph.AddEdge(5, 7);
        graph.AddEdge(5, 8);
        graph.AddEdge(6, 8);
        graph.AddEdge(6, 9);
        graph.AddEdge(7, 9);

        return graph;
    }

    // Hypercube of Order 2
    TNEANet hypercube() {
        TNEANet graph;
        for (int i = 0; i < 8; i++)
            graph.AddNode(i);

        graph.AddEdge(0, 1);
        graph.AddEdge(1, 2);
        graph.AddEdge(2, 3);
        graph.AddEdge(0, 3);

        graph.AddEdge(0, 4);
        graph.AddEdge(1, 5);
        graph.AddEdge(2, 6);
        graph.AddEdge(3, 7);

        graph.AddEdge(4, 5);
        graph.AddEdge(5, 6);
        graph.AddEdge(6, 7);
        graph.AddEdge(7, 4);

        return graph;
    }

    TNEANet wheel(int n) {
        TNEANet graph;

        for (int i = 0; i <= n; i++)
            graph.AddNode(i);

        // Perimeter of wheel
        for (int i = 0; i + 1 < n; i++)
            graph.AddEdge(i, i + 1);

        graph.AddEdge(n - 1, 0); // Complete the perimeter cycle

        // Spokes
        for (int i = 0; i < n; i++)
            graph.AddEdge(i, n);

        return graph;
    }

    TNEANet prism(int n) {
        TNEANet graph;

        // Perimeter: 0 ... n - 1, Inner: n ... 2n - 1
        for (int i = 0; i < 2 * n; i++)
            graph.AddNode(i);

        // Perimeter edges
        for (int i = 0; i + 1 < n; i++)
            graph.AddEdge(i, i + 1);
        graph.AddEdge(n - 1, 0); // Complete cycle

        // Inner edges
        for (int i = n; i + 1 < 2 * n; i++)
            graph.AddEdge(i, i + 1);
        graph.AddEdge(2 * n - 1, n); // Complete cycle

        // Spokes
        for (int i = 0; i < n; i++) {
            graph.AddEdge(i, i + n);
        }

        return graph;
    }

    TNEANet test() {
        TNEANet graph;

        // Add vertices
        const int levels = 2;
        for (int i = 0; i < levels; i++)
            for (int j = 0; j < 3; j++)
                graph.AddNode((i * 3) + j);

        // Connect them
        for (int i = 0; i < (levels - 1) * 3; i++)
            graph.AddEdge(i, i + 3);

        return graph;
    }

    void prism_distances(int min, int max) {
        // Print out distances between points in a prism

        double prev_dist = 0;
        for (int i = min; i <= max; i++) {
            auto graph = prism(i);
            barycenter_layout(graph, i, 1);
            auto x1 = get_xy(graph, i),
                x2 = get_xy(graph, i + 1);

            // Distance between x1 and x2
            std::cout << "Prism on " << i << " vertices: " << sqrt(
                pow((x2.first - x1.first), 2) + 
                pow((x2.second - x1.second), 2)) <<
                " - " << "Change: " << sqrt(
                    pow((x2.first - x1.first), 2) +
                    pow((x2.second - x1.second), 2)) - prev_dist << std::endl;
            prev_dist = sqrt(
                pow((x2.first - x1.first), 2) +
                pow((x2.second - x1.second), 2));
        }
    }
}