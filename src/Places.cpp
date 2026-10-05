// Finding places (search, categories, sorting) and the "you" menus:
// your location, trip history, recent searches, favorites and help.

#include "../include/Navigator.h"
#include "../include/Config.h"
#include "../include/Distance.h"
#include "../include/Search.h"
#include "../include/Sort.h"
#include "../include/Style.h"
#include "../include/Utils.h"

#include <iomanip>
#include <iostream>
#include <vector>

namespace
{
    void numberedPlace(int number, const Location &place)
    {
        std::cout << "  " << style::accent << std::setw(2) << number << style::reset << "  " << std::left
                  << std::setw(36) << place.name << std::right << style::muted << place.category
                  << style::reset << "\n";
    }

    void copyAndReport(const std::string &text)
    {
        if (copyToClipboard(text))
            printSuccess("Copied " + text + " to the clipboard.");
        else
            std::cout << "  Copy this: " << style::strong << text << style::reset << "\n";
    }
}

// ---------------------------------------------------------------------------
// 4. Search
// ---------------------------------------------------------------------------

void Navigator::search()
{
    printTitle("Search", "Type part of a name (\"lib\"), a category (\"hall\"), or a place number.");
    std::string query = readLine("\n  › ");
    if (query.empty())
        return;

    std::vector<int> results;
    int comparisons = 0;
    int id;

    if (parseInt(query, id))
    {
        int index = linearSearchById(campus.locations, id, comparisons);
        if (index != -1)
            results.push_back(index);
        std::cout << "\n  " << style::muted << "Linear search by ID: " << comparisons << " comparisons"
                  << style::reset << "\n";
    }
    else
    {
        // Binary search only works on sorted data, so it runs on a sorted
        // copy and only answers "is there an exact match?".
        std::vector<Location> sorted = campus.locations;
        sortByName(sorted);
        int binarySteps = 0;
        int exact = binarySearchByName(sorted, query, binarySteps);

        // Exact match first, then partial name matches, then category matches.
        if (exact != -1)
        {
            int idComparisons = 0;
            results.push_back(linearSearchById(campus.locations, sorted[exact].id, idComparisons));
        }

        int categoryComparisons = 0;
        std::vector<int> byName = linearSearchByName(campus.locations, query, comparisons);
        std::vector<int> byCategory = linearSearchByCategory(campus.locations, query, categoryComparisons);
        for (const std::vector<int> *group : {&byName, &byCategory})
        {
            for (int index : *group)
            {
                bool already = false;
                for (int r : results)
                    already = already || r == index;
                if (!already)
                    results.push_back(index);
            }
        }

        std::cout << "\n  " << style::muted << "Linear search checked " << comparisons << " places";
        if (exact != -1)
            std::cout << " · binary search found the exact name in " << binarySteps << " steps";
        std::cout << style::reset << "\n";
    }

    logAction("SEARCH", "query=\"" + query + "\" results=" + std::to_string(results.size()));

    if (results.empty())
    {
        printError("Nothing matches \"" + query + "\".");
        pause();
        return;
    }

    std::cout << "\n";
    for (size_t i = 0; i < results.size(); i++)
        numberedPlace(static_cast<int>(i) + 1, campus.locations[results[i]]);

    std::cout << "\n  " << style::muted << "Pick a number to open it, or press Enter to go back." << style::reset;
    int pick = readChoice(static_cast<int>(results.size()));
    if (pick == 0)
        return;

    int chosen = campus.locations[results[pick - 1]].id;
    rememberSearch(chosen);
    openPlace(chosen);
}

void Navigator::openPlace(int id)
{
    while (!inputClosed())
    {
        const Location *place = campus.find(id);
        if (!place)
            return;

        bool favorite = isFavorite(id);

        printTitle((favorite ? "★ " : "") + place->name, place->category + " · number " + std::to_string(id));

        if (!place->description.empty())
            std::cout << "  " << place->description << "\n";

        std::cout << "\n  " << style::muted << "Coordinates  " << style::reset << coordinateString(*place) << "\n";
        if (const Location *here = campus.find(currentLocation))
        {
            if (here->id != id)
                std::cout << "  " << style::muted << "From you     " << style::reset
                          << formatDistance(distanceMeters(*here, *place)) << " straight line\n";
        }

        const std::vector<Edge> &walkways = campus.graph.neighbors(id);
        std::cout << "  " << style::muted << "Walkways     " << style::reset;
        if (walkways.empty())
            std::cout << "none\n";
        for (size_t i = 0; i < walkways.size(); i++)
        {
            std::cout << (i ? "               " : "") << campus.nameOf(walkways[i].to) << style::muted
                      << "  " << walkways[i].meters << " m";
            if (!walkways[i].road.empty())
                std::cout << " on " << walkways[i].road;
            std::cout << style::reset << "\n";
        }

        std::cout << "\n";
        printOption(1, "Take me here");
        printOption(2, "I'm here");
        printOption(3, favorite ? "Remove from favorites" : "Add to favorites");
        printOption(4, "Copy coordinates");
        printOption(0, "Back");
        int choice = readChoice(4);

        if (choice == 0)
            return;

        if (choice == 1)
        {
            int start = currentLocation;
            if (start == -1)
                start = askForPlace("Where are you now?", false);
            if (start == id)
                printSuccess("You're already here.");
            else if (start != -1)
                showTrip(start, id);
        }
        else if (choice == 2)
        {
            moveTo(id);
            printSuccess("Got it, you're at " + place->name + ".");
        }
        else if (choice == 3)
        {
            if (favorite)
                favorites.remove(id);
            else
                favorites.push_back(id);
            saveFavorites();
            printSuccess(favorite ? "Removed from favorites." : "Added to favorites.");
        }
        else
        {
            copyAndReport(coordinateString(*place));
        }
    }
}

// ---------------------------------------------------------------------------
// 5. Browse by category (tree)
// ---------------------------------------------------------------------------

void Navigator::browseCategories()
{
    while (!inputClosed())
    {
        const TreeNode &root = campusTree.root();
        int count = static_cast<int>(root.children.size());

        printTitle("Browse by category", "KUET → category → place, stored as a tree.");
        if (count == 0)
        {
            printWarning("There are no places yet.");
            pause();
            return;
        }

        std::cout << "  " << style::strong << root.name << style::reset << "\n";
        for (int i = 0; i < count; i++)
        {
            const TreeNode &category = root.children[i];
            std::cout << "  " << style::muted << (i + 1 == count ? "└── " : "├── ") << style::reset
                      << style::accent << std::setw(2) << i + 1 << style::reset << "  " << std::left
                      << std::setw(18) << category.name << std::right << style::muted
                      << category.children.size() << (category.children.size() == 1 ? " place" : " places")
                      << style::reset << "\n";
        }
        std::cout << "\n";
        printOption(count + 1, "Show the whole tree");
        printOption(0, "Back");

        int choice = readChoice(count + 1);
        if (choice == 0)
            return;

        if (choice == count + 1)
        {
            printTitle("The campus tree", std::to_string(campusTree.countNodes()) + " nodes, height " +
                                              std::to_string(campusTree.height()));
            campusTree.print();
            pause();
            continue;
        }

        const TreeNode &category = root.children[choice - 1];
        int places = static_cast<int>(category.children.size());
        printTitle(category.name, root.name + " → " + category.name);
        for (int i = 0; i < places; i++)
        {
            std::cout << "  " << style::muted << (i + 1 == places ? "└── " : "├── ") << style::reset
                      << style::accent << std::setw(2) << i + 1 << style::reset << "  "
                      << category.children[i].name << "\n";
        }

        std::cout << "\n  " << style::muted << "Pick a number to open it, or press Enter to go back." << style::reset;
        int pick = readChoice(places);
        if (pick > 0)
            openPlace(category.children[pick - 1].locationId);
    }
}

// ---------------------------------------------------------------------------
// 6. Sort places
// ---------------------------------------------------------------------------

void Navigator::sortPlaces()
{
    printTitle("Sort places");
    printOption(1, "Name A-Z            bubble sort");
    printOption(2, "Name Z-A            selection sort");
    printOption(3, "Category            selection sort");
    printOption(4, "Distance from me    bubble sort");
    printOption(0, "Back");
    int choice = readChoice(4);
    if (choice == 0)
        return;

    SortStats stats;
    std::cout << "\n";

    if (choice == 4)
    {
        const Location *here = campus.find(currentLocation);
        if (!here)
        {
            printWarning("Tell us where you are first (menu 7).");
            pause();
            return;
        }

        std::vector<NearbyPlace> places;
        for (const Location &place : campus.locations)
            places.push_back({place.id, distanceMeters(*here, place)});
        stats = bubbleSortByDistance(places);

        for (size_t i = 0; i < places.size(); i++)
        {
            std::cout << "  " << style::accent << std::setw(2) << i + 1 << style::reset << "  " << std::left
                      << std::setw(36) << campus.nameOf(places[i].locationId) << std::right << style::muted
                      << formatDistance(places[i].meters) << style::reset << "\n";
        }
    }
    else
    {
        // Sort a copy so the stored order doesn't change.
        std::vector<Location> sorted = campus.locations;
        if (choice == 1)
            stats = bubbleSortByName(sorted, true);
        else if (choice == 2)
            stats = selectionSortByName(sorted, false);
        else
            stats = selectionSortByCategory(sorted);

        for (size_t i = 0; i < sorted.size(); i++)
            numberedPlace(static_cast<int>(i) + 1, sorted[i]);
    }

    std::cout << "\n  " << style::muted << stats.comparisons << " comparisons, " << stats.swaps << " swaps"
              << style::reset << "\n";
    pause();
}

// ---------------------------------------------------------------------------
// 7. Where am I?
// ---------------------------------------------------------------------------

void Navigator::whereAmI()
{
    while (!inputClosed())
    {
        const Location *here = campus.find(currentLocation);

        printTitle("Where am I?");
        if (here)
        {
            std::cout << "  You're at " << style::accent << here->name << style::reset << "\n"
                      << "  " << style::muted << "Coordinates  " << style::reset << coordinateString(*here) << "\n";
        }
        else
        {
            std::cout << "  " << style::muted << "We don't know yet. Pick a place or type your coordinates."
                      << style::reset << "\n";
        }

        std::cout << "\n";
        printOption(1, "I'm at a place (pick it)");
        printOption(2, "I'm at these coordinates");
        if (here)
            printOption(3, "Copy my coordinates");
        printOption(0, "Back");
        int choice = readChoice(here ? 3 : 2);

        if (choice == 0)
            return;

        if (choice == 1)
        {
            int id = askForPlace("Which place?", false);
            if (id != -1)
            {
                moveTo(id);
                printSuccess("You're at " + campus.nameOf(id) + ".");
            }
        }
        else if (choice == 2)
        {
            std::string input = readLine("\n  Coordinates, e.g. 22.8992, 89.5016\n  › ");
            double latitude;
            double longitude;
            double meters = 0;
            if (!parseCoordinates(input, latitude, longitude) || !isValidCoordinate(latitude, longitude))
            {
                if (!input.empty())
                    printError("Those don't look like coordinates.");
                continue;
            }

            int id = nearestPlace(latitude, longitude, meters);
            if (id == -1)
                continue;
            std::cout << "\n  Nearest place: " << style::strong << campus.nameOf(id) << style::reset
                      << style::muted << ", about " << formatDistance(meters) << " away" << style::reset << "\n";
            if (meters > 2000)
                printWarning("That's over 2 km away, so you're probably off campus.");
            if (readYesNo("  Set it as your location?", true))
            {
                moveTo(id);
                printSuccess("You're at " + campus.nameOf(id) + ".");
            }
        }
        else
        {
            copyAndReport(coordinateString(*here));
        }
    }
}

// ---------------------------------------------------------------------------
// 8-10. Trip history, recent searches, favorites
// ---------------------------------------------------------------------------

void Navigator::tripHistory()
{
    while (!inputClosed())
    {
        printTitle("Trip history", "Kept on a stack: Back takes you to the last place you were.");
        if (history.isEmpty())
        {
            std::cout << "  " << style::muted << "Empty. Places you go with \"Take me somewhere\" or"
                      << " \"I'm here\" show up here." << style::reset << "\n";
            pause();
            return;
        }

        for (int i = history.size() - 1; i >= 0; i--)
        {
            bool top = (i == history.size() - 1);
            std::cout << "  " << (top ? style::accent : style::muted) << (top ? "▶ " : "  ") << style::reset
                      << campus.nameOf(history.at(i))
                      << (top ? "  (you're here)" : "") << "\n";
        }

        std::cout << "\n";
        printOption(1, "Back (pop the top)");
        printOption(2, "Clear history");
        printOption(0, "Return");
        int choice = readChoice(2);

        if (choice == 0)
            return;

        if (choice == 1)
        {
            int popped;
            history.pop(popped);
            int top;
            currentLocation = history.peek(top) ? top : -1;
            if (currentLocation == -1)
                printSuccess("Left " + campus.nameOf(popped) + ". History is empty now.");
            else
                printSuccess("Back at " + campus.nameOf(currentLocation) + ".");
        }
        else
        {
            history.clear();
            currentLocation = -1;
            printSuccess("History cleared.");
        }
    }
}

void Navigator::showRecentSearches()
{
    printTitle("Recent searches", "A linked list, newest first, last " + std::to_string(MAX_RECENT_SEARCHES) + " kept.");
    if (recentSearches.empty())
    {
        std::cout << "  " << style::muted << "Nothing yet. Places you search for show up here." << style::reset << "\n";
        pause();
        return;
    }

    std::vector<int> ids(recentSearches.begin(), recentSearches.end());
    for (size_t i = 0; i < ids.size(); i++)
        numberedPlace(static_cast<int>(i) + 1, *campus.find(ids[i]));

    std::cout << "\n  " << style::muted << "Pick a number to open it, or press Enter to go back." << style::reset;
    int pick = readChoice(static_cast<int>(ids.size()));
    if (pick > 0)
        openPlace(ids[pick - 1]);
}

void Navigator::showFavorites()
{
    while (!inputClosed())
    {
        printTitle("Favorites");
        if (favorites.empty())
        {
            std::cout << "  " << style::muted << "No favorites yet. Open a place and choose \"Add to favorites\"."
                      << style::reset << "\n";
            pause();
            return;
        }

        std::vector<int> ids(favorites.begin(), favorites.end());
        for (size_t i = 0; i < ids.size(); i++)
            numberedPlace(static_cast<int>(i) + 1, *campus.find(ids[i]));

        std::cout << "\n  " << style::muted << "Pick a number to open it, or press Enter to go back." << style::reset;
        int pick = readChoice(static_cast<int>(ids.size()));
        if (pick == 0)
            return;
        openPlace(ids[pick - 1]);
    }
}

// ---------------------------------------------------------------------------
// 11. Help
// ---------------------------------------------------------------------------

void Navigator::help()
{
    printTitle(std::string("About ") + APP_NAME, std::string(APP_TAGLINE) + " · a KUET campus navigator");

    std::cout << "  " << style::strong << "Getting somewhere" << style::reset << "\n"
                                                                                 "  Choose 1, say where you are and where you're going. You get the roads\n"
                                                                                 "  to take, where to turn, the places you pass, and how long it takes.\n\n"
                                                                                 "  "
              << style::strong << "Typing a place" << style::reset << "\n"
                                                                      "  Anywhere we ask for a place you can type part of its name (\"lib\"),\n"
                                                                      "  its number from the campus map, or coordinates like 22.8992, 89.5016.\n"
                                                                      "  Press Enter on its own to go back, or to use where you are.\n\n"
                                                                      "  "
              << style::strong << "How it works" << style::reset << "\n"
                                                                    "  Places live in an array and walkways in a graph. Routes come from BFS\n"
                                                                    "  on our own queue, exploring uses DFS on our own stack, and so does the\n"
                                                                    "  Back button. Categories are a tree, recent searches a linked list.\n"
                                                                    "  BFS finds the fewest stops, which isn't always the fewest meters.\n\n"
                                                                    "  "
              << style::muted << "Positions come from OpenStreetMap and the KUET master plan (about 30 m).\n"
                                 "  The login is a simple local one for this project, not real security."
              << style::reset << "\n";
    pause();
}
