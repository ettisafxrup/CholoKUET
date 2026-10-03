#pragma once

#include <string>

#include "Location.h"

// Straight-line distance in meters between two points given in decimal
// degrees, using the Haversine formula (Earth radius 6,371 km).
double distanceMeters(double lat1, double lon1, double lat2, double lon2);
double distanceMeters(const Location& a, const Location& b);

// Compass bearing from a to b in degrees: 0 = north, 90 = east.
double bearingDegrees(const Location& a, const Location& b);

// "85 m" under a kilometre, "1.25 km" above.
std::string formatDistance(double meters);
