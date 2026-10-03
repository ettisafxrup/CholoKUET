#pragma once

#include <functional>
#include <list>
#include <map>
#include <string>
#include <vector>

// The campus as an undirected graph: every location is a vertex (keyed by
// its location ID) and every walkway is an edge that knows its length in
// meters and which road it belongs to. Stored as an adjacency list.
//
// BFS uses our own Queue and DFS uses our own Stack (see dsa/).

struct Edge
{
    int to;
    int meters;
    std::string road;
};

class Graph
{
public:
    // Used only for printing traces, e.g. id 3 -> "Central Library".
    void setLabeler(std::function<std::string(int)> labeler) { label = std::move(labeler); }

    bool addVertex(int id);
    void removeVertex(int id);   // also drops every walkway touching it
    bool hasVertex(int id) const;

    // Adds a walkway, or replaces the one already between a and b.
    bool addEdge(int a, int b, int meters, const std::string& road = "");
    bool removeEdge(int a, int b);
    bool hasEdge(int a, int b) const { return findEdge(a, b) != nullptr; }

    // The walkway from a to b, or nullptr if there isn't one.
    const Edge* findEdge(int a, int b) const;

    const std::vector<Edge>& neighbors(int id) const;
    std::vector<int> vertices() const;
    int vertexCount() const { return static_cast<int>(adjacency.size()); }
    int edgeCount() const;
    void clear() { adjacency.clear(); }

    // Route with the fewest stops from start to goal, found with BFS.
    // Empty when no route exists. With `verbose` every queue step is printed.
    std::list<int> findRoute(int start, int goal, bool verbose = false) const;

    // Every place reachable from start, in depth-first order.
    std::vector<int> depthFirstOrder(int start, bool verbose = false) const;

    bool isConnected() const;

private:
    std::map<int, std::vector<Edge>> adjacency;
    std::function<std::string(int)> label;

    std::string nameOf(int id) const;
    void eraseOneWay(int from, int to);
    std::list<int> rebuildRoute(const std::map<int, int>& parent, int start, int goal,
                                bool verbose) const;
};
