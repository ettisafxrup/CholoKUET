#include "../include/Distance.h"

#include <cmath>
#include <cstdio>

double distanceMeters(double lat1, double lon1, double lat2, double lon2)
{
    const double earthRadius = 6371000.0;
    const double toRadians = 3.14159265358979323846 / 180.0;

    double dLat = (lat2 - lat1) * toRadians;
    double dLon = (lon2 - lon1) * toRadians;

    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::cos(lat1 * toRadians) * std::cos(lat2 * toRadians) *
               std::sin(dLon / 2) * std::sin(dLon / 2);

    return earthRadius * 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
}

double distanceMeters(const Location& a, const Location& b)
{
    return distanceMeters(a.latitude, a.longitude, b.latitude, b.longitude);
}

std::string formatDistance(double meters)
{
    char text[32];
    if (meters < 1000.0)
        std::snprintf(text, sizeof(text), "%.0f m", meters);
    else
        std::snprintf(text, sizeof(text), "%.2f km", meters / 1000.0);
    return text;
}

double bearingDegrees(const Location& a, const Location& b)
{
    const double toRadians = 3.14159265358979323846 / 180.0;
    double lat1 = a.latitude * toRadians;
    double lat2 = b.latitude * toRadians;
    double dLon = (b.longitude - a.longitude) * toRadians;

    double y = std::sin(dLon) * std::cos(lat2);
    double x = std::cos(lat1) * std::sin(lat2) - std::sin(lat1) * std::cos(lat2) * std::cos(dLon);
    double degrees = std::atan2(y, x) / toRadians;
    return degrees < 0 ? degrees + 360.0 : degrees;
}
