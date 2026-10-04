#pragma once

#include <list>
#include <string>
#include <vector>

#include "Campus.h"

const int WALKING_METERS_PER_MINUTE = 75;

struct TripStop
{
    int placeId;
    std::string road;
    int metersFromPrevious;
    int metersSoFar;
};

struct TripLeg
{
    std::string road;
    std::string turn;
    std::string heading;
    int from;
    int to;
    int meters;
    std::vector<int> passing;
};

struct Trip
{
    std::vector<TripStop> stops;
    std::vector<TripLeg> legs;
    std::vector<std::string> roads;
    int meters = 0;
    int straightLine = 0;
    int minutes = 0;
};

Trip planTrip(const Campus &campus, const std::list<int> &route);

int walkingMinutes(int meters);

std::string compassName(double bearing);

std::string turnBetween(double fromBearing, double toBearing);
