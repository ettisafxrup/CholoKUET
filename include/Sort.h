#pragma once

#include <vector>

#include "Location.h"

// Bubble sort: keeps swapping neighbours that are out of order, so the
// largest remaining item sinks to the end on every pass. O(n^2), but it
// stops early once a pass makes no swaps. Stable.
//
// Selection sort: each pass picks the smallest remaining item and swaps it
// into place. Always O(n^2) comparisons, but at most n-1 swaps. Not stable.
//
// Names and categories are compared without caring about upper/lower case.

struct SortStats
{
    long comparisons = 0;
    long swaps = 0;
};

struct NearbyPlace
{
    int locationId;
    double meters;
};

SortStats bubbleSortByName(std::vector<Location>& items, bool ascending = true);
SortStats selectionSortByName(std::vector<Location>& items, bool ascending = true);
SortStats selectionSortByCategory(std::vector<Location>& items);
SortStats bubbleSortByDistance(std::vector<NearbyPlace>& items);

// A-Z order, which binary search needs.
void sortByName(std::vector<Location>& items);
