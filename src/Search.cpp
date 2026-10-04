#include "../include/Search.h"
#include "../include/Utils.h"

std::vector<int> linearSearchByName(const std::vector<Location> &locations,
                                    const std::string &query, int &comparisons)
{
    std::vector<int> matches;
    comparisons = 0;
    for (size_t i = 0; i < locations.size(); i++)
    {
        comparisons++;
        if (containsIgnoreCase(locations[i].name, query))
            matches.push_back(static_cast<int>(i));
    }
    return matches;
}

std::vector<int> linearSearchByCategory(const std::vector<Location> &locations,
                                        const std::string &query, int &comparisons)
{
    std::vector<int> matches;
    comparisons = 0;
    for (size_t i = 0; i < locations.size(); i++)
    {
        comparisons++;
        if (containsIgnoreCase(locations[i].category, query))
            matches.push_back(static_cast<int>(i));
    }
    return matches;
}

int linearSearchById(const std::vector<Location> &locations, int id, int &comparisons)
{
    comparisons = 0;
    for (size_t i = 0; i < locations.size(); i++)
    {
        comparisons++;
        if (locations[i].id == id)
            return static_cast<int>(i);
    }
    return -1;
}

bool isSortedByName(const std::vector<Location> &locations)
{
    for (size_t i = 1; i < locations.size(); i++)
    {
        if (compareIgnoreCase(locations[i - 1].name, locations[i].name) > 0)
            return false;
    }
    return true;
}

int binarySearchByName(const std::vector<Location> &sorted, const std::string &name,
                       int &comparisons)
{
    comparisons = 0;
    int low = 0;
    int high = static_cast<int>(sorted.size()) - 1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;
        int result = compareIgnoreCase(name, sorted[mid].name);
        comparisons++;

        if (result == 0)
            return mid;
        if (result < 0)
            high = mid - 1;
        else
            low = mid + 1;
    }
    return -1;
}
