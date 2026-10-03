#include "../include/Distance.h"
#include "../include/Search.h"
#include "../include/Sort.h"
#include "check.h"

static std::vector<Location> named(const std::vector<std::string>& names)
{
    std::vector<Location> items;
    for (size_t i = 0; i < names.size(); i++)
        items.push_back({static_cast<int>(i) + 1, names[i], "Other", "", 0, 0});
    return items;
}

int main()
{
    // empty and single item
    std::vector<Location> items;
    CHECK(bubbleSortByName(items).comparisons == 0);
    CHECK(selectionSortByName(items).comparisons == 0);
    items = named({"Library"});
    CHECK(bubbleSortByName(items).swaps == 0);

    // already sorted: bubble sort stops after one pass
    items = named({"Admin", "CSE", "Gate", "Library"});
    SortStats stats = bubbleSortByName(items);
    CHECK(stats.comparisons == 3 && stats.swaps == 0);

    // reverse sorted: the worst case
    items = named({"Library", "Gate", "CSE", "Admin"});
    stats = bubbleSortByName(items);
    CHECK(isSortedByName(items));
    CHECK(stats.swaps == 6);

    items = named({"Library", "Gate", "CSE", "Admin"});
    stats = selectionSortByName(items);
    CHECK(isSortedByName(items));
    CHECK(stats.comparisons == 6 && stats.swaps <= 3);

    // Z to A
    bubbleSortByName(items, false);
    CHECK(items.front().name == "Library" && items.back().name == "Admin");

    // duplicates: bubble sort keeps equal names in their original order
    items = named({"CSE", "admin", "Admin", "ADMIN"});
    bubbleSortByName(items);
    CHECK(items[0].id == 2 && items[1].id == 3 && items[2].id == 4);

    // by category, then by name
    items = {{1, "Library", "Facility", "", 0, 0}, {2, "EEE", "Academic", "", 0, 0},
             {3, "Auditorium", "Facility", "", 0, 0}, {4, "CSE", "Academic", "", 0, 0}};
    selectionSortByCategory(items);
    CHECK(items[0].name == "CSE" && items[1].name == "EEE" && items[2].name == "Auditorium");

    // by distance
    std::vector<NearbyPlace> places = {{1, 120}, {2, 42}, {3, 85}, {4, 42}, {5, 0}};
    bubbleSortByDistance(places);
    CHECK(places[0].locationId == 5 && places[1].locationId == 2 && places[2].locationId == 4);

    // Haversine: 0.001 degrees of latitude is roughly 111 m anywhere
    double d = distanceMeters(22.900, 89.502, 22.901, 89.502);
    CHECK(d > 110.5 && d < 112.0);

    return finish("Sort");
}
