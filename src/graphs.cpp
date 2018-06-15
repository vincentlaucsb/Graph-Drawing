#include "force_directed.h"

namespace force_directed {
    TUNGraph cycle(int nodes) {
        TUNGraph graph;
        for (int i = 0; i < nodes; i++)
            graph.AddNode(i);

        for (int i = 0; i + 1 < nodes; i++) {
            graph.AddEdge(i, i + 1);
        }

        graph.AddEdge(nodes - 1, 0);
        return graph;
    }

    TUNGraph ladder(int rungs) {
        // Ref: http://mathworld.wolfram.com/LadderGraph.html
        TUNGraph graph;
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

    TUNGraph complete(int nodes) {
        TUNGraph graph;
        for (int i = 0; i < nodes; i++)
            graph.AddNode(i);

        for (int i = 0; i < nodes; i++) {
            for (int j = 0; j < nodes; j++)
                if (i != j) graph.AddEdge(i, j);
        }

        return graph;
    }

    TUNGraph complete_bipartite(int m, int n) {
        TUNGraph graph;
        for (int i = 0; i < m + n; i++) graph.AddNode(i);

        for (int i = 0; i < m; i++) { // Iterate over left side
            // Add edges to right
            for (int j = m; j < m + n; j++) {
                graph.AddEdge(i, j);
            }
        }

        return graph;
    }

    TUNGraph petersen() {
        TUNGraph graph;
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

    TUNGraph generalized_petersen(int n, int k) {
        TUNGraph graph;

        // Vertex sets, where u is the outer vertex set
        std::vector<int> u, v;

        for (int i = 0; i < n * 2; i++) {
            graph.AddNode(i);
            if (i >= n) v.push_back(i);
            else u.push_back(i);
        }

        for (int i = 0; i < n; i++) {
            graph.AddEdge(u[i], u[(i + 1) % n]);
            graph.AddEdge(u[i], v[i]);
            graph.AddEdge(v[i], v[(i + k) % n]);
        }

        return graph;
    }

    TUNGraph hypercube() {
        TUNGraph graph;
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

    TUNGraph hypercube_4() {
        // Hypercube of Order 4
        TUNGraph graph;
        for (int i = 0; i < 16; i++)
            graph.AddNode(i);

        for (int j = 0; j < 2; j++) {
            graph.AddEdge(8 * j + 0, 8 * j + 1);
            graph.AddEdge(8 * j + 1, 8 * j + 2);
            graph.AddEdge(8 * j + 2, 8 * j + 3);
            graph.AddEdge(8 * j + 0, 8 * j + 3);

            graph.AddEdge(8 * j + 0, 8 * j + 4);
            graph.AddEdge(8 * j + 1, 8 * j + 5);
            graph.AddEdge(8 * j + 2, 8 * j + 6);
            graph.AddEdge(8 * j + 3, 8 * j + 7);

            graph.AddEdge(8 * j + 4, 8 * j + 5);
            graph.AddEdge(8 * j + 5, 8 * j + 6);
            graph.AddEdge(8 * j + 6, 8 * j + 7);
            graph.AddEdge(8 * j + 7, 8 * j + 4);
        }

        for (int i = 0; i < 8; i++) {
            graph.AddEdge(i, i + 8);
        }

        return graph;
    }

    TUNGraph tree(int height) {
        // Ternary tree of specified height
        std::map<int, std::vector<int>> children;
        std::deque<int> work = { 0 };

        int total_nodes = 0;
        for (int level = 0; level <= height; level++)
            total_nodes += pow(3, level);

        int current_node = 1, current_parent;
        while (!work.empty() && current_node < total_nodes) {
            current_parent = work.front();
            work.pop_front();

            children[current_parent] = { current_node, current_node + 1, current_node + 2 };
            work.push_back(current_node);
            work.push_back(current_node + 1);
            work.push_back(current_node + 2);
            current_node += 3;
        }

        TUNGraph graph;
        for (int j = 0; j < current_node; j++) graph.AddNode(j);

        for (auto& edge : children) {
            for (auto& dest : edge.second) {
                graph.AddEdge(edge.first, dest);
            }
        }
        
        return graph;
    }

    TUNGraph three_reg_6() {
        // 3-regular graph on 6 vertices
        TUNGraph graph;
        for (int i = 0; i < 6; i++) graph.AddNode(i);

        graph.AddEdge(0, 1);
        graph.AddEdge(0, 3);
        graph.AddEdge(0, 5);

        graph.AddEdge(1, 2);
        graph.AddEdge(1, 4);

        graph.AddEdge(2, 3);
        graph.AddEdge(2, 5);

        graph.AddEdge(3, 4);

        graph.AddEdge(4, 5);

        return graph;
    }

    TUNGraph wheel(int n) {
        TUNGraph graph;

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

    TUNGraph prism(int n) {
        TUNGraph graph;

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
}