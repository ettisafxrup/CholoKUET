// Admin-only menus: editing places and walkways, users, and the activity log.

#include "../include/Navigator.h"
#include "../include/Config.h"
#include "../include/Style.h"
#include "../include/Utils.h"

#include <iomanip>
#include <iostream>
#include <set>

// ---------------------------------------------------------------------------
// 12. Places
// ---------------------------------------------------------------------------

void Navigator::managePlaces()
{
    while (!inputClosed())
    {
        printTitle("Manage places", std::to_string(campus.locations.size()) + " of " +
                                    std::to_string(MAX_LOCATIONS) + " slots used");
        printOption(1, "List all");
        printOption(2, "Add a place");
        printOption(3, "Edit a place");
        printOption(4, "Delete a place");
        printOption(0, "Back");
        int choice = readChoice(4);

        if (choice == 0)
            return;

        if (choice == 1)
        {
            printTitle("All places", "In the order they're stored in the array");
            for (size_t i = 0; i < campus.locations.size(); i++)
            {
                const Location& place = campus.locations[i];
                std::cout << "  " << style::muted << "[" << std::setw(2) << i << "]" << style::reset << "  "
                          << style::accent << std::setw(2) << place.id << style::reset << "  " << std::left
                          << std::setw(36) << place.name << std::right << style::muted << place.category
                          << style::reset << "\n";
            }
            pause();
        }
        else if (choice == 2)
        {
            addPlace();
        }
        else if (choice == 3)
        {
            editPlace();
        }
        else
        {
            deletePlace();
        }
    }
}

void Navigator::addPlace()
{
    printTitle("Add a place");

    Location place;
    int suggested = campus.nextFreeId();
    std::string input = readLine("  Number (Enter = " + std::to_string(suggested) + "): ");
    if (input.empty())
        place.id = suggested;
    else if (!parseInt(input, place.id))
    {
        printError("That isn't a number.");
        pause();
        return;
    }

    place.name = readLine("  Name: ");
    if (place.name.empty())
        return;

    std::cout << "  " << style::muted << "Categories:";
    for (size_t i = 0; i < campus.categories.size(); i++)
        std::cout << (i ? ", " : " ") << campus.categories[i];
    std::cout << style::reset << "\n";
    place.category = readLine("  Category: ");
    if (place.category.empty())
        place.category = "Other";

    place.latitude = readDouble("  Latitude: ");
    place.longitude = readDouble("  Longitude: ");
    place.description = readLine("  Description: ");

    std::string error;
    if (!campus.addLocation(place, error))
    {
        printError(error);
        pause();
        return;
    }

    campus.buildTree(campusTree);
    saveLocations();
    logAction("ADD_LOCATION", "id=" + std::to_string(place.id) + " name=\"" + place.name + "\"");
    printSuccess("Added " + place.name + " as number " + std::to_string(place.id) + ".");
    printWarning("It has no walkways yet. Connect it from Manage walkways (13).");
    pause();
}

void Navigator::editPlace()
{
    printTitle("Edit a place");
    int id = askForPlace("Which place?", false);
    if (id == -1)
        return;

    Location edited = *campus.find(id);
    printLocationDetails(edited);
    std::cout << "\n  " << style::muted << "Press Enter to keep a value as it is." << style::reset << "\n";

    std::string input = readLine("  Name: ");
    if (!input.empty())
        edited.name = input;

    input = readLine("  Category: ");
    if (!input.empty())
        edited.category = input;

    double number;
    input = readLine("  Latitude: ");
    if (!input.empty() && parseDouble(input, number))
        edited.latitude = number;
    else if (!input.empty())
        printWarning("Not a number, keeping the old latitude.");

    input = readLine("  Longitude: ");
    if (!input.empty() && parseDouble(input, number))
        edited.longitude = number;
    else if (!input.empty())
        printWarning("Not a number, keeping the old longitude.");

    input = readLine("  Description: ");
    if (!input.empty())
        edited.description = input;

    std::string error;
    if (!campus.updateLocation(edited, error))
    {
        printError(error);
        pause();
        return;
    }

    campus.buildTree(campusTree);
    saveLocations();
    savePaths();   // walkway lengths change if the place moved
    logAction("EDIT_LOCATION", "id=" + std::to_string(id));
    printSuccess("Saved.");
    pause();
}

void Navigator::deletePlace()
{
    printTitle("Delete a place");
    int id = askForPlace("Which place?", false);
    if (id == -1)
        return;

    std::string name = campus.nameOf(id);
    size_t walkways = campus.graph.neighbors(id).size();
    printWarning(name + " has " + std::to_string(walkways) + " walkway(s). They'll be deleted too.");
    if (!readYesNo("  Delete it?", false))
        return;

    campus.removeLocation(id);
    favorites.remove(id);
    recentSearches.remove(id);

    // A stack only lets us reach the top, so to drop one place from the
    // middle of the history we pop everything onto a second stack and push
    // the rest back. Reversing twice keeps the original order.
    Stack kept;
    int value;
    while (history.pop(value))
    {
        if (value != id)
            kept.push(value);
    }
    while (kept.pop(value))
        history.push(value);

    if (currentLocation == id)
    {
        int top;
        currentLocation = history.peek(top) ? top : -1;
    }

    campus.buildTree(campusTree);
    saveLocations();
    savePaths();
    saveFavorites();
    logAction("DELETE_LOCATION", "id=" + std::to_string(id) + " name=\"" + name + "\"");

    printSuccess("Deleted " + name + ".");
    if (!campus.graph.isConnected())
        printWarning("Some places can no longer be reached from the rest of the campus.");
    pause();
}

// ---------------------------------------------------------------------------
// 13. Walkways
// ---------------------------------------------------------------------------

void Navigator::manageWalkways()
{
    while (!inputClosed())
    {
        printTitle("Manage walkways", std::to_string(campus.graph.edgeCount()) + " walkways");
        printOption(1, "List all");
        printOption(2, "Add a walkway");
        printOption(3, "Remove a walkway");
        printOption(0, "Back");
        int choice = readChoice(3);

        if (choice == 0)
            return;
        if (choice == 1)
            listWalkways();
        else if (choice == 2)
            addWalkway();
        else
            removeWalkway();
    }
}

void Navigator::listWalkways()
{
    printTitle("All walkways");
    int count = 0;
    for (int from : campus.graph.vertices())
    {
        for (const Edge& edge : campus.graph.neighbors(from))
        {
            if (from > edge.to)
                continue;   // each walkway is stored both ways; show it once
            std::cout << "  " << style::muted << std::setw(3) << ++count << style::reset << "  "
                      << campus.nameOf(from) << style::muted << "  ↔  " << style::reset
                      << campus.nameOf(edge.to) << style::muted << "  " << edge.meters << " m"
                      << (edge.road.empty() ? "" : ", " + edge.road) << style::reset << "\n";
        }
    }
    pause();
}

void Navigator::addWalkway()
{
    printTitle("Add a walkway");
    int from = askForPlace("From which place?", false);
    if (from == -1)
        return;
    int to = askForPlace("To which place?", false);
    if (to == -1)
        return;

    if (from == to)
    {
        printError("A walkway needs two different places.");
        pause();
        return;
    }
    if (const Edge* existing = campus.graph.findEdge(from, to))
    {
        printWarning("They're already connected (" + std::to_string(existing->meters) + " m).");
        pause();
        return;
    }

    int straight = campus.pathLength(from, to);
    std::string input = readLine("  Length in meters (Enter = straight line, " + std::to_string(straight) + " m): ");
    int meters = straight;
    if (!input.empty() && (!parseInt(input, meters) || meters <= 0))
    {
        printWarning("Not a valid length, using the straight line.");
        meters = straight;
    }

    std::set<std::string> roads;
    for (int id : campus.graph.vertices())
        for (const Edge& edge : campus.graph.neighbors(id))
            if (!edge.road.empty())
                roads.insert(edge.road);
    std::cout << "  " << style::muted << "Roads so far:";
    for (const std::string& road : roads)
        std::cout << " " << road << ";";
    std::cout << style::reset << "\n";
    std::string road = readLine("  Road name (Enter to leave blank): ");
    if (road.find('|') != std::string::npos)
    {
        printError("Road names can't contain '|'.");
        pause();
        return;
    }

    campus.graph.addEdge(from, to, meters, road);
    savePaths();
    logAction("ADD_PATH", "from=" + std::to_string(from) + " to=" + std::to_string(to));
    printSuccess("Connected " + campus.nameOf(from) + " and " + campus.nameOf(to) + " (" +
                 std::to_string(meters) + " m).");
    pause();
}

void Navigator::removeWalkway()
{
    printTitle("Remove a walkway");
    int from = askForPlace("From which place?", false);
    if (from == -1)
        return;
    int to = askForPlace("To which place?", false);
    if (to == -1)
        return;

    if (!campus.graph.removeEdge(from, to))
    {
        printError("There's no walkway between those two.");
        pause();
        return;
    }

    savePaths();
    logAction("REMOVE_PATH", "from=" + std::to_string(from) + " to=" + std::to_string(to));
    printSuccess("Removed.");
    if (!campus.graph.isConnected())
        printWarning("Some places can no longer be reached from the rest of the campus.");
    pause();
}

// ---------------------------------------------------------------------------
// 14-15. Users and the activity log
// ---------------------------------------------------------------------------

void Navigator::viewUsers()
{
    printTitle("Users", std::to_string(auth.allUsers().size()) + " accounts · passwords are never shown");
    for (const User& user : auth.allUsers())
    {
        std::cout << "  " << std::left << std::setw(30) << user.username << std::right
                  << (user.role == "admin" ? style::accent : style::muted) << user.role << style::reset << "\n";
    }
    pause();
}

void Navigator::viewLog()
{
    printTitle("Activity log", "The most recent entries in logs/application.log");
    logger.printLast(LOG_LINES_TO_SHOW);
    pause();
}
