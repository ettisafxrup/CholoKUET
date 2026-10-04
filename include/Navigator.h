#pragma once

#include <list>
#include <string>

#include "Authentication.h"
#include "Campus.h"
#include "CampusTree.h"
#include "Logger.h"
#include "../dsa/stack/Stack.h"

class Navigator
{
public:
    void initialize();
    void run();

private:
    std::string baseDir;
    Authentication auth;
    Campus campus;
    CampusTree campusTree;
    Logger logger;

    Stack history;                 // places you've been, newest on top
    std::list<int> recentSearches; // newest first
    std::list<int> favorites;
    int currentLocation = -1;

    // Setup and saving (Navigator.cpp)
    std::string dataPath(const std::string &relative) const;
    void loadData();
    void reportLoad(const std::string &what, const LoadResult &result);
    void saveLocations();
    void savePaths();
    void saveFavorites();

    // Session (Navigator.cpp)
    bool loginScreen();
    bool login();
    void registerAccount();
    void logout();
    void mainMenu();
    void handleChoice(int choice);

    // Shared helpers (Navigator.cpp)
    int resolvePlace(const std::string &input);
    int askForPlace(const std::string &question, bool enterMeansCurrent);
    bool askForPoint(const std::string &question, double &latitude, double &longitude);
    int nearestPlace(double latitude, double longitude, double &meters) const;
    void moveTo(int id);
    void rememberSearch(int id);
    bool isFavorite(int id) const;
    void logAction(const std::string &action, const std::string &details);

    // Getting around (Trips.cpp)
    void takeMeSomewhere();
    void showTrip(int start, int goal);
    void nearMe();
    void explore();

    // Finding places (Places.cpp)
    void search();
    void openPlace(int id);
    void browseCategories();
    void sortPlaces();

    // You (Places.cpp)
    void whereAmI();
    void tripHistory();
    void showRecentSearches();
    void showFavorites();
    void help();

    // Admin (NavigatorAdmin.cpp)
    void managePlaces();
    void addPlace();
    void editPlace();
    void deletePlace();
    void manageWalkways();
    void listWalkways();
    void addWalkway();
    void removeWalkway();
    void viewUsers();
    void viewLog();
};
