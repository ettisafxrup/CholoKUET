#pragma once

#include <list>
#include <string>
#include <vector>

#include "CampusTree.h"
#include "Graph.h"
#include "Location.h"

struct LoadResult
{
    bool missing = false;
    int loaded = 0;
    int skipped = 0;
};

class Campus
{
public:
    std::vector<Location> locations;
    std::vector<std::string> categories;
    Graph graph;

    Campus();
    Campus(const Campus &) = delete;
    Campus &operator=(const Campus &) = delete;

    Location *find(int id);
    const Location *find(int id) const;
    std::string nameOf(int id) const;
    int nextFreeId() const;

    bool addLocation(const Location &location, std::string &error);
    bool updateLocation(const Location &location, std::string &error);
    void removeLocation(int id);

    int pathLength(int a, int b) const;

    void buildTree(CampusTree &tree) const;

    LoadResult loadCategories(const std::string &path);
    LoadResult loadLocations(const std::string &path);
    LoadResult loadPaths(const std::string &path);
    bool saveLocations(const std::string &path) const;
    bool savePaths(const std::string &path) const;

    std::list<int> loadFavorites(const std::string &path, const std::string &username) const;
    bool saveFavorites(const std::string &path, const std::string &username,
                       const std::list<int> &favorites) const;

private:
    bool validate(const Location &location, std::string &error) const;
    void addCategory(const std::string &category);
};
