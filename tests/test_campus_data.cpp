
#include "../include/Campus.h"
#include "../include/Config.h"
#include "check.h"

int main()
{
    Campus campus;
    CHECK(!campus.loadCategories(CATEGORIES_FILE).missing);

    LoadResult locations = campus.loadLocations(LOCATIONS_FILE);
    CHECK(!locations.missing);
    CHECK(locations.skipped == 0);
    CHECK(locations.loaded >= 40);

    LoadResult paths = campus.loadPaths(PATHS_FILE);
    CHECK(!paths.missing);
    CHECK(paths.skipped == 0);

    for (int id : campus.graph.vertices())
        for (const Edge &edge : campus.graph.neighbors(id))
            CHECK(!edge.road.empty());

    CHECK(campus.graph.isConnected());

    CHECK(campus.find(3) != nullptr);
    for (const Location &place : campus.locations)
        CHECK(campus.pathLength(3, place.id) < 1000);

    CHECK(!campus.graph.findRoute(1, 3).empty());

    return finish("Campus data");
}
