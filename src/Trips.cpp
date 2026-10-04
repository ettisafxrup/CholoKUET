
#include "../include/Navigator.h"
#include "../include/Distance.h"
#include "../include/Sort.h"
#include "../include/Style.h"
#include "../include/Trip.h"
#include "../include/Utils.h"

#include <iomanip>
#include <iostream>
#include <map>

namespace
{
    std::string minutesText(int minutes)
    {
        if (minutes <= 0)
            return "no time at all";
        if (minutes == 1)
            return "about 1 min";
        return "about " + std::to_string(minutes) + " min";
    }

    std::string legInstruction(const TripLeg &leg)
    {
        if (leg.turn == "Start")
            return "Head " + leg.heading + " on";
        if (leg.turn == "Continue straight")
            return "Continue " + leg.heading + " onto";
        return leg.turn + ", heading " + leg.heading + " on";
    }

    std::string joinNames(const Campus &campus, const std::vector<int> &ids)
    {
        std::string text;
        for (size_t i = 0; i < ids.size(); i++)
        {
            if (i > 0)
                text += (i + 1 == ids.size()) ? " and " : ", ";
            text += campus.nameOf(ids[i]);
        }
        return text;
    }

    void printTrip(const Campus &campus, const Trip &trip)
    {
        int start = trip.stops.front().placeId;
        int goal = trip.stops.back().placeId;

        std::cout << "\n  " << style::heading << campus.nameOf(start) << "  →  " << campus.nameOf(goal)
                  << style::reset << "\n  " << style::strong << formatDistance(trip.meters) << style::reset
                  << style::muted << " walk · " << style::reset << style::strong << minutesText(trip.minutes)
                  << style::reset << style::muted << " · " << trip.stops.size() << " places · "
                  << trip.roads.size() << (trip.roads.size() == 1 ? " road" : " roads") << style::reset << "\n";
        printRule();

        std::cout << "\n  " << style::muted << "DIRECTIONS" << style::reset << "\n\n";
        std::cout << "   " << style::good << "●" << style::reset << "  Start at "
                  << style::strong << campus.nameOf(start) << style::reset << "\n";

        int step = 1;
        for (const TripLeg &leg : trip.legs)
        {
            std::cout << "   " << style::muted << "│" << style::reset << "\n"
                      << "  " << style::accent << std::setw(2) << step++ << style::reset << "  "
                      << legInstruction(leg) << " "
                      << style::accent << leg.road << style::reset << style::muted << "  ·  "
                      << formatDistance(leg.meters) << ", " << minutesText(walkingMinutes(leg.meters))
                      << style::reset << "\n";
            if (!leg.passing.empty())
                std::cout << "   " << style::muted << "│  past " << joinNames(campus, leg.passing)
                          << style::reset << "\n";
            std::cout << "   " << style::muted << "│  " << style::reset << "until you reach "
                      << campus.nameOf(leg.to) << "\n";
        }

        std::cout << "   " << style::muted << "│" << style::reset << "\n"
                  << "   " << style::good << "●" << style::reset << "  Arrive at "
                  << style::strong << campus.nameOf(goal) << style::reset << "\n";

        std::cout << "\n  " << style::muted << "STOPS" << style::reset << "\n\n"
                  << style::muted << "   #   " << std::left << std::setw(36) << "Place" << std::setw(24)
                  << "Road" << std::right << std::setw(7) << "Leg" << std::setw(9) << "Total"
                  << style::reset << "\n";
        for (size_t i = 0; i < trip.stops.size(); i++)
        {
            const TripStop &stop = trip.stops[i];
            bool endpoint = (i == 0 || i + 1 == trip.stops.size());
            std::cout << "  " << std::setw(2) << i + 1 << "   ";
            if (endpoint)
                std::cout << style::strong;
            std::cout << std::left << std::setw(36) << campus.nameOf(stop.placeId) << style::reset
                      << style::muted << std::setw(24) << (stop.road.empty() ? "-" : stop.road) << std::right
                      << std::setw(7) << (i == 0 ? "-" : formatDistance(stop.metersFromPrevious))
                      << style::reset << std::setw(9) << formatDistance(stop.metersSoFar) << "\n";
        }

        std::cout << "\n  " << style::muted << "ROADS  " << style::reset;
        for (size_t i = 0; i < trip.roads.size(); i++)
        {
            if (i > 0)
                std::cout << style::muted << "  →  " << style::reset;
            std::cout << trip.roads[i];
        }
        std::cout << "\n  " << style::muted << "Straight-line distance " << formatDistance(trip.straightLine)
                  << ". BFS picks the route with the fewest stops, which isn't always the shortest walk."
                  << style::reset << "\n";
    }
}

void Navigator::takeMeSomewhere()
{
    printTitle("Take me somewhere",
               "Type a place name (or part of it), its number from the map, or coordinates.");

    int start = askForPlace("Where are you now?", true);
    if (start == -1)
        return;

    int goal = askForPlace("Where do you want to go?", false);
    if (goal == -1)
        return;

    if (start == goal)
    {
        printSuccess("You're already at " + campus.nameOf(goal) + ".");
        moveTo(goal);
        pause();
        return;
    }

    showTrip(start, goal);
}

void Navigator::showTrip(int start, int goal)
{
    std::list<int> route = campus.graph.findRoute(start, goal);
    logAction("NAVIGATION", "start=" + std::to_string(start) + " goal=" + std::to_string(goal) +
                                " result=" + (route.empty() ? "no_route" : "found"));

    if (route.empty())
    {
        printError("There's no walkway connecting " + campus.nameOf(start) + " and " + campus.nameOf(goal) + ".");
        pause();
        return;
    }

    Trip trip = planTrip(campus, route);
    printTrip(campus, trip);

    while (!inputClosed())
    {
        std::cout << "\n   " << style::accent << "1" << style::reset << "  I'm going, set "
                  << campus.nameOf(goal) << " as my location\n"
                  << "   " << style::accent << "2" << style::reset << "  Show how BFS found this route\n"
                  << "   " << style::accent << "0" << style::reset << "  Back\n";
        int choice = readChoice(2);

        if (choice == 0)
            return;

        if (choice == 1)
        {
            moveTo(start);
            moveTo(goal);
            printSuccess("Have a good walk! You're now at " + campus.nameOf(goal) + ".");
            pause();
            return;
        }

        printTitle("How BFS found it", "Our queue explores the campus one ring of neighbours at a time.");
        campus.graph.findRoute(start, goal, true);
        pause();
    }
}

void Navigator::nearMe()
{
    printTitle("What's near me", "A place name, its number, or coordinates like 22.8992, 89.5016");

    double latitude;
    double longitude;
    if (!askForPoint("Where are you?", latitude, longitude))
        return;

    std::vector<NearbyPlace> places;
    for (const Location &place : campus.locations)
        places.push_back({place.id, distanceMeters(latitude, longitude, place.latitude, place.longitude)});
    SortStats stats = bubbleSortByDistance(places);

    Location point;
    point.latitude = latitude;
    point.longitude = longitude;

    int shown = static_cast<int>(places.size()) < 10 ? static_cast<int>(places.size()) : 10;
    std::cout << "\n";
    for (int i = 0; i < shown; i++)
    {
        const Location &place = *campus.find(places[i].locationId);
        std::cout << "  " << style::accent << std::setw(2) << i + 1 << style::reset << "  " << std::left
                  << std::setw(36) << place.name << std::right << std::setw(8) << formatDistance(places[i].meters)
                  << style::muted << "  ";
        if (places[i].meters < 10)
            std::cout << "you're here";
        else
            std::cout << std::left << std::setw(11) << compassName(bearingDegrees(point, place)) << std::right
                      << minutesText(walkingMinutes(static_cast<int>(places[i].meters)));
        std::cout << style::reset << "\n";
    }
    std::cout << "\n  " << style::muted << "Sorted with bubble sort: " << stats.comparisons
              << " comparisons, " << stats.swaps << " swaps." << style::reset << "\n";
    if (!places.empty() && places[0].meters > 2000)
        printWarning("The closest place is over 2 km away, so you're probably off campus.");

    std::cout << "\n  " << style::muted << "Pick a number to open it, or press Enter to go back." << style::reset;
    int pick = readChoice(shown);
    if (pick > 0)
        openPlace(places[pick - 1].locationId);
}

void Navigator::explore()
{
    while (!inputClosed())
    {
        printTitle("Explore the campus");
        std::cout << "   " << style::accent << "1" << style::reset << "  Walk everywhere from one place (DFS)\n"
                  << "   " << style::accent << "2" << style::reset << "  Can every place be reached?\n"
                  << "   " << style::accent << "3" << style::reset << "  All roads and walkways\n"
                  << "   " << style::accent << "0" << style::reset << "  Back\n";
        int choice = readChoice(3);

        if (choice == 0)
            return;

        if (choice == 1)
        {
            int start = askForPlace("Start from where?", true);
            if (start == -1)
                continue;

            bool verbose = readYesNo("  Show the stack at every step?", false);
            std::vector<int> order = campus.graph.depthFirstOrder(start, verbose);

            printTitle("DFS from " + campus.nameOf(start),
                       "The order a depth-first walk reaches every place.");
            for (size_t i = 0; i < order.size(); i++)
                std::cout << "  " << style::accent << std::setw(2) << i + 1 << style::reset << "  "
                          << campus.nameOf(order[i]) << "\n";
            std::cout << "\n  Reached " << style::strong << order.size() << " of " << campus.graph.vertexCount()
                      << style::reset << " places.\n";

            logAction("DFS", "start=" + std::to_string(start) + " reached=" + std::to_string(order.size()));
            pause();
        }
        else if (choice == 2)
        {
            std::vector<int> all = campus.graph.vertices();
            if (all.empty())
            {
                printWarning("There are no places yet.");
                pause();
                continue;
            }

            std::vector<int> reached = campus.graph.depthFirstOrder(all[0]);
            std::cout << "\n  DFS from " << campus.nameOf(all[0]) << " reached " << reached.size()
                      << " of " << all.size() << " places.\n";

            if (reached.size() == all.size())
            {
                printSuccess("Every place on campus can be reached from every other.");
            }
            else
            {
                printWarning("These places can't be reached:");
                for (int id : all)
                {
                    bool found = false;
                    for (int r : reached)
                        found = found || r == id;
                    if (!found)
                        std::cout << "     " << campus.nameOf(id) << "\n";
                }
            }
            pause();
        }
        else
        {
            std::map<std::string, std::vector<const Edge *>> byRoad;
            std::map<const Edge *, int> fromOf;
            for (int from : campus.graph.vertices())
            {
                for (const Edge &edge : campus.graph.neighbors(from))
                {
                    if (from < edge.to)
                    {
                        byRoad[edge.road.empty() ? "Other walkways" : edge.road].push_back(&edge);
                        fromOf[&edge] = from;
                    }
                }
            }

            printTitle("Roads and walkways", std::to_string(campus.graph.edgeCount()) + " walkways on " +
                                                 std::to_string(byRoad.size()) + " roads");
            for (const auto &road : byRoad)
            {
                int total = 0;
                for (const Edge *edge : road.second)
                    total += edge->meters;
                std::cout << "\n  " << style::accent << road.first << style::reset << style::muted
                          << "  ·  " << formatDistance(total) << style::reset << "\n";
                for (const Edge *edge : road.second)
                    std::cout << "     " << campus.nameOf(fromOf[edge]) << style::muted << "  ↔  "
                              << style::reset << campus.nameOf(edge->to) << style::muted << "  "
                              << edge->meters << " m" << style::reset << "\n";
            }
            pause();
        }
    }
}
