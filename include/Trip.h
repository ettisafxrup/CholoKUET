#pragma once

#include <list>
#include <string>
#include <vector>

#include "Campus.h"

// Turns a BFS route (a list of place IDs) into something you can follow:
// the roads to take, which way to head, where to turn, the places you pass,
// and how far and how long it is.

const int WALKING_METERS_PER_MINUTE = 75;   // about 4.5 km/h

struct TripStop
{
    int placeId;
    std::string road;         // road taken to get here, empty for the start
    int metersFromPrevious;
    int metersSoFar;
};

// One stretch of the trip along a single road.
struct TripLeg
{
    std::string road;
    std::string turn;       // "Start", "Turn left", "Continue straight", ...
    std::string heading;    // "north-east", ...
    int from;
    int to;
    int meters;
    std::vector<int> passing;   // places passed on the way, not counting from/to
};

struct Trip
{
    std::vector<TripStop> stops;
    std::vector<TripLeg> legs;
    std::vector<std::string> roads;   // each road once, in the order used
    int meters = 0;
    int straightLine = 0;
    int minutes = 0;
};

Trip planTrip(const Campus& campus, const std::list<int>& route);

int walkingMinutes(int meters);

// Compass direction of a bearing: 0 = "north", 90 = "east", ...
std::string compassName(double bearing);

// "Turn left", "Turn right", "Continue straight" or "Turn around", for a
// change of direction from one bearing to the next.
std::string turnBetween(double fromBearing, double toBearing);
