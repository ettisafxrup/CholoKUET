#include "../include/Trip.h"
#include "check.h"

// A tiny campus laid out on a grid (0.001° ≈ 111 m north, 102 m east):
//
//   4 Library ─── 5 EEE          (North Road)
//   │
//   3 CSE                         (Main Road, runs north)
//   │
//   2 Admin
//   │
//   1 Gate
int main()
{
    Campus campus;
    std::string error;
    campus.addLocation({1, "Gate", "Transportation", "", 22.8970, 89.5000}, error);
    campus.addLocation({2, "Admin", "Administrative", "", 22.8980, 89.5000}, error);
    campus.addLocation({3, "CSE", "Academic", "", 22.8990, 89.5000}, error);
    campus.addLocation({4, "Library", "Facility", "", 22.9000, 89.5000}, error);
    campus.addLocation({5, "EEE", "Academic", "", 22.9000, 89.5010}, error);
    campus.graph.addEdge(1, 2, 110, "Main Road");
    campus.graph.addEdge(2, 3, 110, "Main Road");
    campus.graph.addEdge(3, 4, 110, "Main Road");
    campus.graph.addEdge(4, 5, 100, "North Road");

    // compass names and turns
    CHECK(compassName(0) == "north");
    CHECK(compassName(44) == "north-east");
    CHECK(compassName(90) == "east");
    CHECK(compassName(359) == "north");
    CHECK(turnBetween(0, 90) == "Turn right");
    CHECK(turnBetween(0, 270) == "Turn left");
    CHECK(turnBetween(10, 350) == "Continue straight");
    CHECK(turnBetween(0, 180) == "Turn around");

    // walking time rounds up to whole minutes
    CHECK(walkingMinutes(0) == 0);
    CHECK(walkingMinutes(1) == 1);
    CHECK(walkingMinutes(75) == 1);
    CHECK(walkingMinutes(76) == 2);

    // an empty route gives an empty trip
    CHECK(planTrip(campus, {}).stops.empty());

    // Gate -> EEE: three stops on Main Road merge into one leg, then a right turn
    Trip trip = planTrip(campus, campus.graph.findRoute(1, 5));
    CHECK(trip.stops.size() == 5);
    CHECK(trip.meters == 430);
    CHECK(trip.stops.back().metersSoFar == 430);
    CHECK(trip.stops[1].road == "Main Road");
    CHECK(trip.minutes == 6);

    CHECK(trip.legs.size() == 2);
    CHECK(trip.legs[0].road == "Main Road");
    CHECK(trip.legs[0].turn == "Start");
    CHECK(trip.legs[0].heading == "north");
    CHECK(trip.legs[0].meters == 330);
    CHECK((trip.legs[0].passing == std::vector<int>{2, 3}));
    CHECK(trip.legs[0].to == 4);

    CHECK(trip.legs[1].road == "North Road");
    CHECK(trip.legs[1].turn == "Turn right");
    CHECK(trip.legs[1].heading == "east");
    CHECK(trip.legs[1].passing.empty());

    CHECK((trip.roads == std::vector<std::string>{"Main Road", "North Road"}));
    CHECK(trip.straightLine > 340 && trip.straightLine < 360);   // about sqrt(334² + 102²)

    // the same trip backwards turns left instead
    Trip back = planTrip(campus, campus.graph.findRoute(5, 1));
    CHECK(back.legs.size() == 2);
    CHECK(back.legs[0].heading == "west");
    CHECK(back.legs[1].turn == "Turn left");

    return finish("Trip");
}
