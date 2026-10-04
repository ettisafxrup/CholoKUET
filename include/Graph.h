#pragma once

#include <functional>
#include <list>
#include <map>
#include <string>
#include <vector>

struct Edge
{
    int to;
    int meters;
    std::string road;
};

class Graph
{
public:
    void setLabeler(std::function<std::string(int)> labeler) { label = std::move(labeler); }

    bool addVertex(int id);
    void removeVertex(int id);
    bool hasVertex(int id) const;

    bool addEdge(int a, int b, int meters, const std::string &road = "");
    bool removeEdge(int a, int b);
    bool hasEdge(int a, int b) const { return findEdge(a, b) != nullptr; }

    const Edge *findEdge(int a, int b) const;

    const std::vector<Edge> &neighbors(int id) const;
    std::vector<int> vertices() const;
    int vertexCount() const { return static_cast<int>(adjacency.size()); }
    int edgeCount() const;
    void clear() { adjacency.clear(); }

    std::list<int> findRoute(int start, int goal, bool verbose = false) const;

    std::vector<int> depthFirstOrder(int start, bool verbose = false) const;

    bool isConnected() const;

private:
    std::map<int, std::vector<Edge>> adjacency;
    std::function<std::string(int)> label;

    std::string nameOf(int id) const;
    void eraseOneWay(int from, int to);
    std::list<int> rebuildRoute(const std::map<int, int> &parent, int start, int goal,
                                bool verbose) const;
};
