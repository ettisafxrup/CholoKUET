#include "../include/Location.h"
#include "../include/Utils.h"

#include <cstdio>
#include <iostream>
#include <vector>

bool isValidCoordinate(double latitude, double longitude)
{
    return latitude >= -90.0 && latitude <= 90.0 && longitude >= -180.0 && longitude <= 180.0;
}

bool parseLocation(const std::string &line, Location &location, std::string &error)
{
    std::vector<std::string> fields = split(line, '|');
    if (fields.size() < 5)
    {
        error = "expected ID|Name|Category|Latitude|Longitude|Description";
        return false;
    }

    Location parsed;
    if (!parseInt(fields[0], parsed.id) || parsed.id <= 0)
    {
        error = "ID must be a positive number";
        return false;
    }
    if (fields[1].empty())
    {
        error = "name is empty";
        return false;
    }
    if (!parseDouble(fields[3], parsed.latitude) || !parseDouble(fields[4], parsed.longitude) ||
        !isValidCoordinate(parsed.latitude, parsed.longitude))
    {
        error = "invalid coordinates";
        return false;
    }

    parsed.name = fields[1];
    parsed.category = fields[2].empty() ? "Other" : fields[2];

    for (size_t i = 5; i < fields.size(); i++)
        parsed.description += (i > 5 ? "|" : "") + fields[i];

    location = parsed;
    return true;
}

std::string formatLocation(const Location &location)
{
    char coordinates[64];
    std::snprintf(coordinates, sizeof(coordinates), "%.6f|%.6f", location.latitude, location.longitude);
    return std::to_string(location.id) + "|" + location.name + "|" + location.category + "|" +
           coordinates + "|" + location.description;
}

std::string coordinateString(const Location &location)
{
    char text[64];
    std::snprintf(text, sizeof(text), "%.6f, %.6f", location.latitude, location.longitude);
    return text;
}

void printLocationDetails(const Location &location)
{
    char latitude[32];
    char longitude[32];
    std::snprintf(latitude, sizeof(latitude), "%.6f", location.latitude);
    std::snprintf(longitude, sizeof(longitude), "%.6f", location.longitude);

    std::cout << "  ID          : " << location.id << "\n"
              << "  Name        : " << location.name << "\n"
              << "  Category    : " << location.category << "\n"
              << "  Description : " << (location.description.empty() ? "-" : location.description) << "\n"
              << "  Latitude    : " << latitude << "\n"
              << "  Longitude   : " << longitude << "\n"
              << "  Share       : " << coordinateString(location) << "\n";
}
