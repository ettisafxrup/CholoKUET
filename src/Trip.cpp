#include "../include/Trip.h"
#include "../include/Distance.h"

#include <cmath>

int walkingMinutes(int meters)
{
    if (meters <= 0)
        return 0;
    return (meters + WALKING_METERS_PER_MINUTE - 1) / WALKING_METERS_PER_MINUTE;
}

std::string compassName(double bearing)
{
    static const char* const names[] = {"north", "north-east", "east", "south-east",
                                        "south", "south-west", "west", "north-west"};
    int sector = static_cast<int>(std::floor(std::fmod(bearing + 22.5 + 360.0, 360.0) / 45.0));
    return names[sector % 8];
}

std::string turnBetween(double fromBearing, double toBearing)
{
    // Positive = clockwise = to the right.
    double change = std::fmod(toBearing - fromBearing + 540.0, 360.0) - 180.0;
    if (std::fabs(change) < 35.0)
        return "Continue straight";
    if (std::fabs(change) > 150.0)
        return "Turn around";
    return change > 0 ? "Turn right" : "Turn left";
}

static double bearingOf(const Campus& campus, int from, int to)
{
    return bearingDegrees(*campus.find(from), *campus.find(to));
}

Trip planTrip(const Campus& campus, const std::list<int>& route)
{
    Trip trip;
    if (route.empty())
        return trip;

    int previous = -1;
    double lastBearing = 0.0;

    for (int id : route)
    {
        if (previous == -1)
        {
            trip.stops.push_back({id, "", 0, 0});
            previous = id;
            continue;
        }

        const Edge* edge = campus.graph.findEdge(previous, id);
        int meters = edge ? edge->meters : campus.pathLength(previous, id);
        std::string road = (edge && !edge->road.empty()) ? edge->road : "the campus walkway";
        double bearing = bearingOf(campus, previous, id);

        trip.meters += meters;
        trip.stops.push_back({id, road, meters, trip.meters});

        if (!trip.legs.empty() && trip.legs.back().road == road)
        {
            // Same road as before: the last stop becomes a place we pass.
            TripLeg& leg = trip.legs.back();
            leg.passing.push_back(leg.to);
            leg.to = id;
            leg.meters += meters;
        }
        else
        {
            TripLeg leg;
            leg.road = road;
            leg.turn = trip.legs.empty() ? "Start" : turnBetween(lastBearing, bearing);
            leg.from = previous;
            leg.to = id;
            leg.meters = meters;
            trip.legs.push_back(leg);

            bool seen = false;
            for (const std::string& known : trip.roads)
                seen = seen || known == road;
            if (!seen)
                trip.roads.push_back(road);
        }

        lastBearing = bearing;
        previous = id;
    }

    // A leg's heading is the overall direction from where it starts to
    // where it ends, which reads better than the first segment's direction.
    for (TripLeg& leg : trip.legs)
        leg.heading = compassName(bearingOf(campus, leg.from, leg.to));

    trip.straightLine = static_cast<int>(std::lround(
        distanceMeters(*campus.find(route.front()), *campus.find(route.back()))));
    trip.minutes = walkingMinutes(trip.meters);
    return trip;
}
