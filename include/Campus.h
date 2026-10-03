#pragma once

#include <list>
#include <string>
#include <vector>

#include "CampusTree.h"
#include "Graph.h"
#include "Location.h"

// Counts from loading one data file, so the app can warn about bad lines.
struct LoadResult
{
    bool missing = false;
    int loaded = 0;
    int skipped = 0;
};

// Holds all campus data (locations in an array, categories, the path
// graph) and is the only place that reads or writes the data files.
class Campus
{
public:
    std::vector<Location> locations;
    std::vector<std::string> categories;
    Graph graph;

    Campus();
    Campus(const Campus&) = delete;              // the graph's labeler points back at us
    Campus& operator=(const Campus&) = delete;

    Location* find(int id);
    const Location* find(int id) const;
    std::string nameOf(int id) const;
    int nextFreeId() const;

    bool addLocation(const Location& location, std::string& error);
    bool updateLocation(const Location& location, std::string& error);
    void removeLocation(int id);

    // Straight-line length in meters, used when a path has no length given.
    int pathLength(int a, int b) const;

    void buildTree(CampusTree& tree) const;

    LoadResult loadCategories(const std::string& path);
    LoadResult loadLocations(const std::string& path);
    LoadResult loadPaths(const std::string& path);
    bool saveLocations(const std::string& path) const;
    bool savePaths(const std::string& path) const;

    // favorites.txt holds "username|locationId" lines for every user.
    std::list<int> loadFavorites(const std::string& path, const std::string& username) const;
    bool saveFavorites(const std::string& path, const std::string& username,
                       const std::list<int>& favorites) const;

private:
    bool validate(const Location& location, std::string& error) const;
    void addCategory(const std::string& category);
};
