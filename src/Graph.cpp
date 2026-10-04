#include "../include/Graph.h"
#include "../include/Config.h"
#include "../include/Style.h"
#include "../dsa/queue/Queue.h"
#include "../dsa/stack/Stack.h"

#include <iostream>
#include <set>

std::string Graph::nameOf(int id) const
{
    return label ? label(id) : "#" + std::to_string(id);
}

bool Graph::addVertex(int id)
{
    // Our Stack and Queue hold at most MAX_LOCATIONS items, so the graph
    // can't grow past that or BFS/DFS could overflow them.
    if (hasVertex(id) || vertexCount() >= MAX_LOCATIONS)
        return false;

    adjacency[id];
    return true;
}

void Graph::removeVertex(int id)
{
    auto it = adjacency.find(id);
    if (it == adjacency.end())
        return;

    for (const Edge &edge : it->second)
        eraseOneWay(edge.to, id);
    adjacency.erase(it);
}

bool Graph::hasVertex(int id) const
{
    return adjacency.count(id) > 0;
}

bool Graph::addEdge(int a, int b, int meters, const std::string &road)
{
    if (a == b || !hasVertex(a) || !hasVertex(b))
        return false;

    removeEdge(a, b);
    adjacency[a].push_back({b, meters, road});
    adjacency[b].push_back({a, meters, road});
    return true;
}

bool Graph::removeEdge(int a, int b)
{
    if (!hasEdge(a, b))
        return false;

    eraseOneWay(a, b);
    eraseOneWay(b, a);
    return true;
}

void Graph::eraseOneWay(int from, int to)
{
    std::vector<Edge> &edges = adjacency[from];
    for (auto it = edges.begin(); it != edges.end(); ++it)
    {
        if (it->to == to)
        {
            edges.erase(it);
            return;
        }
    }
}

const Edge *Graph::findEdge(int a, int b) const
{
    for (const Edge &edge : neighbors(a))
    {
        if (edge.to == b)
            return &edge;
    }
    return nullptr;
}

const std::vector<Edge> &Graph::neighbors(int id) const
{
    static const std::vector<Edge> none;
    auto it = adjacency.find(id);
    return it == adjacency.end() ? none : it->second;
}

std::vector<int> Graph::vertices() const
{
    std::vector<int> ids;
    for (const auto &entry : adjacency)
        ids.push_back(entry.first);
    return ids;
}

int Graph::edgeCount() const
{
    int total = 0;
    for (const auto &entry : adjacency)
        total += static_cast<int>(entry.second.size());
    return total / 2; // every walkway is stored once in each direction
}

// Implemented BFS
std::list<int> Graph::findRoute(int start, int goal, bool verbose) const
{
    if (!hasVertex(start) || !hasVertex(goal))
        return {};

    auto nameLabel = [this](int id)
    { return nameOf(id); };

    std::set<int> visited;
    std::map<int, int> parent;
    Queue queue;

    queue.enqueue(start);
    visited.insert(start);
    parent[start] = -1;

    if (verbose)
    {
        std::cout << "  Start at " << nameOf(start) << "\n";
        queue.print(nameLabel);
    }

    int step = 0;
    while (!queue.isEmpty())
    {
        int current;
        queue.dequeue(current);

        if (verbose)
            std::cout << "\n"
                      << style::accent << "  Step " << ++step << style::reset
                      << ": dequeue " << nameOf(current) << "\n";

        if (current == goal)
            break;

        for (const Edge &edge : neighbors(current))
        {
            if (visited.count(edge.to))
                continue;

            visited.insert(edge.to);
            parent[edge.to] = current;
            queue.enqueue(edge.to);

            if (verbose)
                std::cout << "     enqueue " << nameOf(edge.to) << style::muted
                          << "  (parent: " << nameOf(current) << ")" << style::reset << "\n";
        }

        if (verbose)
            queue.print(nameLabel);
    }

    if (!visited.count(goal))
    {
        if (verbose)
            std::cout << "\n  The queue ran out before reaching the destination.\n";
        return {};
    }

    return rebuildRoute(parent, start, goal, verbose);
}

std::list<int> Graph::rebuildRoute(const std::map<int, int> &parent, int start, int goal,
                                   bool verbose) const
{
    // Following parent[] from the goal gives the route backwards. Pushing
    // it onto a stack and popping it off turns it the right way round.
    Stack stack;
    for (int at = goal; at != -1; at = parent.at(at))
        stack.push(at);

    if (verbose)
    {
        std::cout << "\n  Walking back through parent[] from " << nameOf(goal)
                  << " to " << nameOf(start) << ", pushing each stop:\n";
        stack.print([this](int id)
                    { return nameOf(id); });
        std::cout << "  Popping the stack gives the route from the start.\n";
    }

    std::list<int> route;
    int id;
    while (stack.pop(id))
        route.push_back(id);
    return route;
}

// Implemented DFS
std::vector<int> Graph::depthFirstOrder(int start, bool verbose) const
{
    std::vector<int> order;
    if (!hasVertex(start))
        return order;

    auto nameLabel = [this](int id)
    { return nameOf(id); };

    std::set<int> visited;
    std::map<int, size_t> nextEdge;
    Stack stack;

    stack.push(start);
    visited.insert(start);
    order.push_back(start);

    if (verbose)
    {
        std::cout << "  push " << nameOf(start) << "  (visit #1)\n";
        stack.printInline(nameLabel);
    }

    while (!stack.isEmpty())
    {
        int current;
        stack.peek(current);

        const std::vector<Edge> &edges = neighbors(current);
        int next = -1;
        while (nextEdge[current] < edges.size())
        {
            int candidate = edges[nextEdge[current]++].to;
            if (!visited.count(candidate))
            {
                next = candidate;
                break;
            }
        }

        if (next != -1)
        {
            stack.push(next);
            visited.insert(next);
            order.push_back(next);

            if (verbose)
            {
                std::cout << "  push " << nameOf(next) << "  (visit #" << order.size() << ")\n";
                stack.printInline(nameLabel);
            }
        }
        else
        {
            stack.pop(current);
            if (verbose)
                std::cout << style::muted << "  pop  " << nameOf(current)
                          << "  (nothing new next to it, go back)" << style::reset << "\n";
        }
    }

    return order;
}

bool Graph::isConnected() const
{
    if (adjacency.empty())
        return true;
    return static_cast<int>(depthFirstOrder(adjacency.begin()->first).size()) == vertexCount();
}
