#include "../include/Campus.h"
#include "../include/Config.h"
#include "../include/Distance.h"
#include "../include/Utils.h"

#include <cmath>
#include <fstream>

Campus::Campus()
{
    graph.setLabeler([this](int id)
                     { return nameOf(id); });
}

Location *Campus::find(int id)
{
    for (Location &location : locations)
    {
        if (location.id == id)
            return &location;
    }
    return nullptr;
}

const Location *Campus::find(int id) const
{
    for (const Location &location : locations)
    {
        if (location.id == id)
            return &location;
    }
    return nullptr;
}

std::string Campus::nameOf(int id) const
{
    const Location *location = find(id);
    return location ? location->name : "(unknown #" + std::to_string(id) + ")";
}

int Campus::nextFreeId() const
{
    int id = 1;
    while (find(id))
        id++;
    return id;
}

bool Campus::validate(const Location &location, std::string &error) const
{
    if (location.id <= 0)
        error = "ID must be a positive number.";
    else if (trim(location.name).empty())
        error = "Name can't be empty.";
    else if ((location.name + location.category + location.description).find('|') != std::string::npos)
        error = "'|' is used by the data files, please leave it out.";
    else if (!isValidCoordinate(location.latitude, location.longitude))
        error = "Latitude must be -90..90 and longitude -180..180.";
    else
        return true;
    return false;
}

void Campus::addCategory(const std::string &category)
{
    for (const std::string &existing : categories)
    {
        if (equalsIgnoreCase(existing, category))
            return;
    }
    categories.push_back(category);
}

bool Campus::addLocation(const Location &location, std::string &error)
{
    if (!validate(location, error))
        return false;
    if (find(location.id))
    {
        error = "Another location already uses that ID.";
        return false;
    }
    if (static_cast<int>(locations.size()) >= MAX_LOCATIONS)
    {
        error = "The campus already has the maximum number of locations.";
        return false;
    }

    locations.push_back(location);
    graph.addVertex(location.id);
    addCategory(location.category);
    return true;
}

bool Campus::updateLocation(const Location &location, std::string &error)
{
    if (!validate(location, error))
        return false;

    Location *existing = find(location.id);
    if (!existing)
    {
        error = "No location has that ID.";
        return false;
    }

    bool moved = existing->latitude != location.latitude || existing->longitude != location.longitude;
    *existing = location;
    addCategory(location.category);

    if (moved)
    {
        for (const Edge &edge : std::vector<Edge>(graph.neighbors(location.id)))
            graph.addEdge(location.id, edge.to, pathLength(location.id, edge.to), edge.road);
    }
    return true;
}

void Campus::removeLocation(int id)
{
    for (size_t i = 0; i < locations.size(); i++)
    {
        if (locations[i].id == id)
        {
            locations.erase(locations.begin() + i);
            break;
        }
    }
    graph.removeVertex(id);
}

int Campus::pathLength(int a, int b) const
{
    const Location *from = find(a);
    const Location *to = find(b);
    if (!from || !to)
        return 1;

    int meters = static_cast<int>(std::lround(distanceMeters(*from, *to)));
    return meters < 1 ? 1 : meters;
}

void Campus::buildTree(CampusTree &tree) const
{
    tree.build(categories, locations);
}

LoadResult Campus::loadCategories(const std::string &path)
{
    LoadResult result;
    std::ifstream file(path);
    if (!file)
    {
        result.missing = true;
        return result;
    }

    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;
        addCategory(line);
        result.loaded++;
    }
    return result;
}

LoadResult Campus::loadLocations(const std::string &path)
{
    LoadResult result;
    std::ifstream file(path);
    if (!file)
    {
        result.missing = true;
        return result;
    }

    locations.clear();
    graph.clear();

    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        Location location;
        std::string error;
        if (parseLocation(line, location, error) && addLocation(location, error))
            result.loaded++;
        else
            result.skipped++;
    }
    return result;
}

LoadResult Campus::loadPaths(const std::string &path)
{
    LoadResult result;
    std::ifstream file(path);
    if (!file)
    {
        result.missing = true;
        return result;
    }

    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> fields = split(line, '|');
        int from = 0;
        int to = 0;
        if (fields.size() < 2 || !parseInt(fields[0], from) || !parseInt(fields[1], to) ||
            graph.hasEdge(from, to))
        {
            result.skipped++;
            continue;
        }

        int meters = 0;
        if (fields.size() < 3 || !parseInt(fields[2], meters) || meters < 1)
            meters = pathLength(from, to);
        std::string road = fields.size() >= 4 ? fields[3] : "";

        if (graph.addEdge(from, to, meters, road))
            result.loaded++;
        else
            result.skipped++;
    }
    return result;
}

bool Campus::saveLocations(const std::string &path) const
{
    std::ofstream file(path);
    if (!file)
        return false;

    file << "# ID|Name|Category|Latitude|Longitude|Description\n"
         << "#\n"
         << "# Where these positions come from:\n"
         << "#  - Most buildings: OpenStreetMap (c) OpenStreetMap contributors, ODbL.\n"
         << "#  - Places marked \"(plan)\" aren't mapped in OSM yet. They were measured off\n"
         << "#    the KUET master plan, which we lined up with OSM using 18 buildings that\n"
         << "#    appear in both (average difference about 27 m).\n"
         << "# See docs/campus-map.md for the details.\n";
    for (const Location &location : locations)
        file << formatLocation(location) << "\n";
    return static_cast<bool>(file);
}

bool Campus::savePaths(const std::string &path) const
{
    std::ofstream file(path);
    if (!file)
        return false;

    file << "# FromID|ToID|Meters|Road   (every walkway works both ways)\n"
         << "# Road names are our own descriptive labels, except KUET Road and\n"
         << "# Khanjahan Ali Hall Road, which are the names used in OpenStreetMap.\n";
    for (int from : graph.vertices())
    {
        for (const Edge &edge : graph.neighbors(from))
        {
            if (from < edge.to) // write each walkway once
                file << from << "|" << edge.to << "|" << edge.meters << "|" << edge.road << "\n";
        }
    }
    return static_cast<bool>(file);
}

std::list<int> Campus::loadFavorites(const std::string &path, const std::string &username) const
{
    std::list<int> favorites;
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> fields = split(line, '|');
        int id = 0;
        if (fields.size() < 2 || fields[0] != username || !parseInt(fields[1], id) || !find(id))
            continue;

        bool alreadyThere = false;
        for (int existing : favorites)
            alreadyThere = alreadyThere || existing == id;
        if (!alreadyThere)
            favorites.push_back(id);
    }
    return favorites;
}

bool Campus::saveFavorites(const std::string &path, const std::string &username,
                           const std::list<int> &favorites) const
{
    // Keep everyone else's favorites, replace this user's.
    std::vector<std::string> otherLines;
    {
        std::ifstream in(path);
        std::string line;
        while (std::getline(in, line))
        {
            line = trim(line);
            if (!line.empty() && line[0] != '#' && split(line, '|')[0] != username)
                otherLines.push_back(line);
        }
    }

    std::ofstream out(path);
    if (!out)
        return false;

    out << "# username|locationId\n";
    for (const std::string &line : otherLines)
        out << line << "\n";
    for (int id : favorites)
        out << username << "|" << id << "\n";
    return static_cast<bool>(out);
}
