#include "../include/Config.h"
#include "../include/Graph.h"
#include "check.h"

//  1 Gate - 2 Admin - 3 CSE - 4 Library - 5 EEE
//                                |
//                           6 Auditorium        7 (on its own)
static void build(Graph& graph)
{
    graph.clear();
    for (int id = 1; id <= 7; id++)
        graph.addVertex(id);
    graph.addEdge(1, 2, 60);
    graph.addEdge(2, 3, 70);
    graph.addEdge(3, 4, 80);
    graph.addEdge(4, 5, 90);
    graph.addEdge(4, 6, 50);
}

int main()
{
    Graph graph;
    build(graph);

    // paths go both ways
    CHECK(graph.vertexCount() == 7);
    CHECK(graph.edgeCount() == 5);
    CHECK(graph.hasEdge(2, 1));
    CHECK(graph.findEdge(4, 6) && graph.findEdge(4, 6)->meters == 50);
    CHECK(graph.findEdge(1, 3) == nullptr);
    CHECK(!graph.addEdge(1, 1, 5));
    CHECK(!graph.addEdge(1, 99, 5));
    CHECK(graph.neighbors(4).size() == 3);

    // BFS: a reachable destination
    CHECK((graph.findRoute(1, 6) == std::list<int>{1, 2, 3, 4, 6}));

    // BFS: start and destination are the same
    CHECK((graph.findRoute(3, 3) == std::list<int>{3}));

    // BFS: unreachable or unknown destination
    CHECK(graph.findRoute(1, 7).empty());
    CHECK(graph.findRoute(1, 42).empty());

    // BFS goes for the fewest stops, even if that path is longer in meters
    graph.addEdge(1, 4, 500);
    CHECK((graph.findRoute(1, 5) == std::list<int>{1, 4, 5}));
    graph.removeEdge(1, 4);
    CHECK(!graph.hasEdge(4, 1));

    // DFS on a disconnected graph
    CHECK((graph.depthFirstOrder(1) == std::vector<int>{1, 2, 3, 4, 5, 6}));
    CHECK(!graph.isConnected());
    CHECK(graph.depthFirstOrder(7).size() == 1);
    CHECK(graph.depthFirstOrder(99).empty());

    // DFS on a connected graph
    graph.addEdge(6, 7, 40);
    CHECK(graph.isConnected());

    // removing a place also removes its paths
    graph.removeVertex(2);
    CHECK(!graph.hasVertex(2));
    CHECK(graph.neighbors(1).empty());
    CHECK(graph.findEdge(4, 5) && graph.findEdge(4, 5)->meters == 90);
    CHECK(graph.edgeCount() == 4);

    // the biggest possible fully connected graph must not overflow our
    // fixed-size Stack or Queue
    graph.clear();
    for (int id = 1; id <= MAX_LOCATIONS; id++)
        graph.addVertex(id);
    CHECK(!graph.addVertex(MAX_LOCATIONS + 1));
    for (int a = 1; a <= MAX_LOCATIONS; a++)
        for (int b = a + 1; b <= MAX_LOCATIONS; b++)
            graph.addEdge(a, b, 1);
    CHECK(static_cast<int>(graph.depthFirstOrder(1).size()) == MAX_LOCATIONS);
    CHECK(graph.findRoute(1, MAX_LOCATIONS).size() == 2);

    return finish("Graph, BFS and DFS");
}
