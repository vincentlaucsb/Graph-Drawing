#include "graphs.h"

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