#pragma once

#include <string>

struct Location
{
    int id = 0;
    std::string name;
    std::string category;
    std::string description;
    double latitude = 0.0;
    double longitude = 0.0;
};

bool isValidCoordinate(double latitude, double longitude);

bool parseLocation(const std::string &line, Location &location, std::string &error);
std::string formatLocation(const Location &location);

std::string coordinateString(const Location &location);

void printLocationDetails(const Location &location);
