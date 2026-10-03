// Loads the real files in data/ and checks they make sense together.
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

    // every walkway belongs to a named road
    for (int id : campus.graph.vertices())
        for (const Edge& edge : campus.graph.neighbors(id))
            CHECK(!edge.road.empty());

    // every place can be reached from every other place
    CHECK(campus.graph.isConnected());

    // nothing is placed off campus (all within 1 km of the Central Library)
    CHECK(campus.find(3) != nullptr);
    for (const Location& place : campus.locations)
        CHECK(campus.pathLength(3, place.id) < 1000);

    // the route used in the presentation exists
    CHECK(!campus.graph.findRoute(1, 3).empty());

    return finish("Campus data");
}
