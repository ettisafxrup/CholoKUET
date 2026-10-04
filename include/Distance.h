#pragma once

#include <string>

#include "Location.h"

double distanceMeters(double lat1, double lon1, double lat2, double lon2);
double distanceMeters(const Location &a, const Location &b);

double bearingDegrees(const Location &a, const Location &b);

std::string formatDistance(double meters);
