#include "../include/Sort.h"
#include "../include/Utils.h"

#include <utility>

static bool outOfOrder(const Location& a, const Location& b, bool ascending)
{
    int result = compareIgnoreCase(a.name, b.name);
    return ascending ? result > 0 : result < 0;
}

SortStats bubbleSortByName(std::vector<Location>& items, bool ascending)
{
    SortStats stats;
    int n = static_cast<int>(items.size());

    for (int pass = 0; pass < n - 1; pass++)
    {
        bool swapped = false;

        // After each pass the last `pass + 1` items are in their final place.
        for (int j = 0; j < n - 1 - pass; j++)
        {
            stats.comparisons++;
            if (outOfOrder(items[j], items[j + 1], ascending))
            {
                std::swap(items[j], items[j + 1]);
                stats.swaps++;
                swapped = true;
            }
        }

        if (!swapped)
            break;   // nothing moved, so it's already sorted
    }
    return stats;
}

SortStats selectionSortByName(std::vector<Location>& items, bool ascending)
{
    SortStats stats;
    int n = static_cast<int>(items.size());

    for (int i = 0; i < n - 1; i++)
    {
        // Find the item that belongs at position i.
        int best = i;
        for (int j = i + 1; j < n; j++)
        {
            stats.comparisons++;
            if (outOfOrder(items[best], items[j], ascending))
                best = j;
        }

        if (best != i)
        {
            std::swap(items[i], items[best]);
            stats.swaps++;
        }
    }
    return stats;
}

SortStats selectionSortByCategory(std::vector<Location>& items)
{
    SortStats stats;
    int n = static_cast<int>(items.size());

    for (int i = 0; i < n - 1; i++)
    {
        int best = i;
        for (int j = i + 1; j < n; j++)
        {
            stats.comparisons++;
            int byCategory = compareIgnoreCase(items[j].category, items[best].category);
            if (byCategory < 0 || (byCategory == 0 && compareIgnoreCase(items[j].name, items[best].name) < 0))
                best = j;
        }

        if (best != i)
        {
            std::swap(items[i], items[best]);
            stats.swaps++;
        }
    }
    return stats;
}

SortStats bubbleSortByDistance(std::vector<NearbyPlace>& items)
{
    SortStats stats;
    int n = static_cast<int>(items.size());

    for (int pass = 0; pass < n - 1; pass++)
    {
        bool swapped = false;
        for (int j = 0; j < n - 1 - pass; j++)
        {
            stats.comparisons++;
            if (items[j].meters > items[j + 1].meters)
            {
                std::swap(items[j], items[j + 1]);
                stats.swaps++;
                swapped = true;
            }
        }
        if (!swapped)
            break;
    }
    return stats;
}

void sortByName(std::vector<Location>& items)
{
    bubbleSortByName(items);
}
