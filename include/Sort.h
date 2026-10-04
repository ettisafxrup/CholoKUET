#pragma once

#include <vector>

#include "Location.h"

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

SortStats bubbleSortByName(std::vector<Location> &items, bool ascending = true);
SortStats selectionSortByName(std::vector<Location> &items, bool ascending = true);
SortStats selectionSortByCategory(std::vector<Location> &items);
SortStats bubbleSortByDistance(std::vector<NearbyPlace> &items);

void sortByName(std::vector<Location> &items);
