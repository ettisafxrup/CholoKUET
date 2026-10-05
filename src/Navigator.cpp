// Setup, the login screen, the main menu and helpers shared by every menu.

#include "../include/Navigator.h"
#include "../include/Config.h"
#include "../include/Distance.h"
#include "../include/Search.h"
#include "../include/Style.h"
#include "../include/Utils.h"

#include <iomanip>
#include <iostream>

std::string Navigator::dataPath(const std::string &relative) const
{
    return baseDir + relative;
}

void Navigator::initialize()
{
    initConsole();

    for (const char *candidate : {"", "../", "../../", "../../../"})
    {
        if (fileExists(std::string(candidate) + LOCATIONS_FILE))
        {
            baseDir = candidate;
            break;
        }
    }

    makeDirectory(dataPath("data"));
    makeDirectory(dataPath("logs"));
    logger.open(dataPath(LOG_FILE));
    logger.log("STARTUP");

    loadData();
}

void Navigator::reportLoad(const std::string &what, const LoadResult &result)
{
    if (result.missing)
    {
        printWarning("No " + what + " file found, starting with none.");
        logger.log("ERROR", "missing_file=" + what);
        return;
    }
    if (result.skipped > 0)
        printWarning("Skipped " + std::to_string(result.skipped) + " bad or repeated lines in the " + what + " file.");

    logger.log("LOAD", what + "=" + std::to_string(result.loaded) + " skipped=" + std::to_string(result.skipped));
}

void Navigator::loadData()
{
    // Categories first, so the tree keeps the order written in the file.
    reportLoad("categories", campus.loadCategories(dataPath(CATEGORIES_FILE)));
    reportLoad("places", campus.loadLocations(dataPath(LOCATIONS_FILE)));
    reportLoad("walkways", campus.loadPaths(dataPath(PATHS_FILE)));
    campus.buildTree(campusTree);

    if (!auth.load(dataPath(USERS_FILE)) || auth.allUsers().empty())
    {
        auth.addDefaultUsers();
        auth.save(dataPath(USERS_FILE));
        printWarning("No accounts yet, so two were made: admin / admin123 and student1 / student123");
    }

    if (!campus.graph.isConnected())
        printWarning("Some places have no walkway to the rest of the campus.");
}

void Navigator::saveLocations()
{
    if (!campus.saveLocations(dataPath(LOCATIONS_FILE)))
        printError("Couldn't save the places file.");
}

void Navigator::savePaths()
{
    if (!campus.savePaths(dataPath(PATHS_FILE)))
        printError("Couldn't save the walkways file.");
}

void Navigator::saveFavorites()
{
    if (auth.loggedIn())
        campus.saveFavorites(dataPath(FAVORITES_FILE), auth.currentUser().username, favorites);
}

void Navigator::run()
{
    while (loginScreen())
    {
        while (auth.loggedIn())
        {
            mainMenu();
            int choice = readChoice(auth.isAdmin() ? 15 : 11, false);
            if (choice == 0 || inputClosed())
                logout();
            else
                handleChoice(choice);
        }
    }

    logger.log("SHUTDOWN");
    std::cout << "\n  " << style::accent << APP_NAME << style::reset << " · "
              << APP_TAGLINE << ". See you around campus!\n\n";
}

// ---------------------------------------------------------------------------
// Login screen
// ---------------------------------------------------------------------------

namespace
{
    void banner()
    {
        std::cout << "\n"
                  << style::heading
                  << "   ╔═══════════════════════════════════════╗\n"
                  << "   ║               CholoKUET               ║\n"
                  << "   ╚═══════════════════════════════════════╝" << style::reset << "\n"
                  << style::muted << "       Cholo Explore Kore Ashi · KUET campus" << style::reset << "\n";
    }

    void menuItem(int number, const std::string &label, const std::string &hint = "")
    {
        std::cout << "   " << style::accent << std::setw(2) << number << style::reset << "  "
                  << std::left << std::setw(24) << label << std::right;
        if (!hint.empty())
            std::cout << style::muted << hint << style::reset;
        std::cout << "\n";
    }

    void menuGroup(const std::string &name)
    {
        std::cout << "\n  " << style::muted << name << style::reset << "\n";
    }
}

bool Navigator::loginScreen()
{
    while (!inputClosed())
    {
        style::clearScreen();
        banner();
        std::cout << "\n";
        menuItem(1, "Log in");
        menuItem(2, "Create an account");
        menuItem(0, "Exit");

        int choice = readChoice(2, false);
        if (choice == 0)
            return false;
        if (choice == 1 && login())
            return true;
        if (choice == 2)
            registerAccount();
    }
    return false;
}

bool Navigator::login()
{
    printTitle("Log in");
    std::string username = readLine("  Username: ");
    std::string password = readPassword("  Password: ");

    if (!auth.login(username, password))
    {
        printError("That username and password don't match.");
        logger.log("LOGIN", "user=" + username + " result=failed");
        pause();
        return false;
    }

    const User &user = auth.currentUser();
    logger.log("LOGIN", "user=" + user.username + " result=success");

    history.clear();
    recentSearches.clear();
    currentLocation = -1;
    favorites = campus.loadFavorites(dataPath(FAVORITES_FILE), user.username);
    return true;
}

void Navigator::registerAccount()
{
    printTitle("Create an account", "Username: 3-30 letters, digits, _ or .   Password: 4 or more characters");

    std::string username = readLine("  Username: ");
    std::string password = readPassword("  Password: ");
    std::string again = readPassword("  Password again: ");

    std::string error;
    if (password != again)
    {
        printError("The two passwords don't match.");
    }
    else if (!auth.registerUser(username, password, "student", error))
    {
        printError(error);
    }
    else
    {
        auth.save(dataPath(USERS_FILE));
        logger.log("REGISTER", "user=" + username);
        printSuccess("Account created. You can log in now.");
    }
    pause();
}

void Navigator::logout()
{
    std::string username = auth.currentUser().username;
    saveFavorites();
    auth.logout();

    favorites.clear();
    recentSearches.clear();
    history.clear();
    currentLocation = -1;

    logger.log("LOGOUT", "user=" + username);
}

void Navigator::mainMenu()
{
    style::clearScreen();
    banner();

    std::cout << "\n  Hi, " << style::strong << auth.currentUser().username << style::reset;
    if (const Location *here = campus.find(currentLocation))
        std::cout << "  ·  you're at " << style::accent << here->name << style::reset;
    else
        std::cout << style::muted << "  ·  tell us where you are with 7" << style::reset;
    std::cout << "\n";

    menuGroup("GET AROUND");
    menuItem(1, "Take me somewhere", "from where you are to where you want to go");
    menuItem(2, "What's near me", "closest places, nearest first");
    menuItem(3, "Explore the campus", "walk the whole campus with DFS");

    menuGroup("FIND");
    menuItem(4, "Search", "by name, category or ID");
    menuItem(5, "Browse by category", "the campus as a tree");
    menuItem(6, "Sort places", "A-Z, Z-A, category, distance");

    menuGroup("YOU");
    menuItem(7, "Where am I?", "set or share your location");
    menuItem(8, "Trip history", "go back to where you were");
    menuItem(9, "Recent searches");
    menuItem(10, "Favorites", std::to_string(favorites.size()) + " saved");
    menuItem(11, "Help");

    if (auth.isAdmin())
    {
        menuGroup("ADMIN");
        menuItem(12, "Manage places");
        menuItem(13, "Manage walkways");
        menuItem(14, "Users");
        menuItem(15, "Activity log");
    }

    std::cout << "\n";
    menuItem(0, "Log out");
}

void Navigator::handleChoice(int choice)
{
    switch (choice)
    {
    case 1:
        takeMeSomewhere();
        break;
    case 2:
        nearMe();
        break;
    case 3:
        explore();
        break;
    case 4:
        search();
        break;
    case 5:
        browseCategories();
        break;
    case 6:
        sortPlaces();
        break;
    case 7:
        whereAmI();
        break;
    case 8:
        tripHistory();
        break;
    case 9:
        showRecentSearches();
        break;
    case 10:
        showFavorites();
        break;
    case 11:
        help();
        break;
    case 12:
        managePlaces();
        break;
    case 13:
        manageWalkways();
        break;
    case 14:
        viewUsers();
        break;
    case 15:
        viewLog();
        break;
    }
}

int Navigator::nearestPlace(double latitude, double longitude, double &meters) const
{
    int best = -1;
    for (const Location &place : campus.locations)
    {
        double d = distanceMeters(latitude, longitude, place.latitude, place.longitude);
        if (best == -1 || d < meters)
        {
            best = place.id;
            meters = d;
        }
    }
    return best;
}

int Navigator::resolvePlace(const std::string &input)
{
    double latitude;
    double longitude;
    if (parseCoordinates(input, latitude, longitude))
    {
        if (!isValidCoordinate(latitude, longitude))
        {
            printError("Those coordinates are out of range.");
            return -1;
        }
        double meters = 0;
        int id = nearestPlace(latitude, longitude, meters);
        if (id != -1)
        {
            std::cout << style::muted << "  Closest known place: " << style::reset << campus.nameOf(id)
                      << style::muted << " (" << formatDistance(meters) << " away)" << style::reset << "\n";
            if (meters > 2000)
                printWarning("That's over 2 km from anything on campus.");
        }
        return id;
    }

    int id;
    if (parseInt(input, id))
    {
        if (campus.find(id))
        {
            std::cout << style::muted << "  → " << style::reset << campus.nameOf(id) << "\n";
            return id;
        }
        printError("No place has number " + std::to_string(id) + ".");
        return -1;
    }

    int comparisons;
    std::vector<int> matches = linearSearchByName(campus.locations, input, comparisons);
    if (matches.empty())
        matches = linearSearchByCategory(campus.locations, input, comparisons);

    if (matches.empty())
    {
        printError("Nothing matches \"" + input + "\". Try part of a name, like \"lib\" or \"hall\".");
        return -1;
    }
    if (matches.size() == 1)
    {
        std::cout << style::muted << "  → " << style::reset << campus.locations[matches[0]].name << "\n";
        return campus.locations[matches[0]].id;
    }

    std::cout << style::muted << "  Which one?" << style::reset << "\n";
    for (size_t i = 0; i < matches.size(); i++)
    {
        std::cout << "   " << style::accent << std::setw(2) << i + 1 << style::reset << "  "
                  << campus.locations[matches[i]].name << "\n";
    }
    int pick = readChoice(static_cast<int>(matches.size()));
    return pick > 0 ? campus.locations[matches[pick - 1]].id : -1;
}

int Navigator::askForPlace(const std::string &question, bool enterMeansCurrent)
{
    const Location *here = campus.find(currentLocation);
    bool useHere = enterMeansCurrent && here;

    while (!inputClosed())
    {
        std::cout << "\n  " << style::strong << question << style::reset;
        if (useHere)
            std::cout << style::muted << "  (Enter = " << here->name << ")" << style::reset;
        std::string input = readLine("\n  › ");

        if (input.empty())
            return useHere ? currentLocation : -1;

        int id = resolvePlace(input);
        if (id != -1)
            return id;
    }
    return -1;
}

bool Navigator::askForPoint(const std::string &question, double &latitude, double &longitude)
{
    const Location *here = campus.find(currentLocation);

    std::cout << "\n  " << style::strong << question << style::reset;
    if (here)
        std::cout << style::muted << "  (Enter = " << here->name << ")" << style::reset;
    std::string input = readLine("\n  › ");

    if (input.empty())
    {
        if (!here)
            return false;
        latitude = here->latitude;
        longitude = here->longitude;
        return true;
    }

    if (parseCoordinates(input, latitude, longitude))
    {
        if (isValidCoordinate(latitude, longitude))
            return true;
        printError("Those coordinates are out of range.");
        return false;
    }

    const Location *place = campus.find(resolvePlace(input));
    if (!place)
        return false;
    latitude = place->latitude;
    longitude = place->longitude;
    return true;
}

void Navigator::moveTo(int id)
{
    int top;
    if (history.peek(top) && top == id)
    {
        currentLocation = id;
        return;
    }
    if (!history.push(id))
    {
        printWarning("Trip history is full. Clear it from menu 8 to keep tracking.");
        return;
    }
    currentLocation = id;
}

void Navigator::rememberSearch(int id)
{
    recentSearches.remove(id);
    recentSearches.push_front(id);
    if (recentSearches.size() > static_cast<size_t>(MAX_RECENT_SEARCHES))
        recentSearches.pop_back();
}

bool Navigator::isFavorite(int id) const
{
    for (int favorite : favorites)
    {
        if (favorite == id)
            return true;
    }
    return false;
}

void Navigator::logAction(const std::string &action, const std::string &details)
{
    logger.log(action, "user=" + auth.currentUser().username + " " + details);
}
