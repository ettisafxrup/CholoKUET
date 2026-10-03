#pragma once

#include <string>

struct Location
{
    int id = 0;
    std::string name;
    std::string category;
    std::string description;
    double latitude = 0.0;     // decimal degrees
    double longitude = 0.0;
};

bool isValidCoordinate(double latitude, double longitude);

// Reads one "ID|Name|Category|Latitude|Longitude|Description" line.
// On failure `error` says what was wrong with it.
bool parseLocation(const std::string& line, Location& location, std::string& error);
std::string formatLocation(const Location& location);

// "22.900100, 89.502300" - ready to paste into any map app.
std::string coordinateString(const Location& location);

void printLocationDetails(const Location& location);
