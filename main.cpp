// CholoKUET - Cholo Explore Kore Ashi
// A KUET campus navigator for our DSA lab.
//
// Build and run with make:  make run
// Or with one command:      g++ -std=c++17 main.cpp -o CholoKUET

#include "include/Navigator.h"

int main()
{
    Navigator app;
    app.initialize();
    app.run();
    return 0;
}

// When only this file is compiled (plain "g++ main.cpp", or an editor's
// Run button), pull in the rest of the program so everything still links.
// The Makefile and CMake compile each file on its own and define
// CHOLOKUET_SEPARATE_FILES to skip this.
#ifndef CHOLOKUET_SEPARATE_FILES
#include "dsa/stack/Stack.cpp"
#include "dsa/queue/Queue.cpp"
#include "src/Location.cpp"
#include "src/Distance.cpp"
#include "src/Graph.cpp"
#include "src/CampusTree.cpp"
#include "src/Campus.cpp"
#include "src/Search.cpp"
#include "src/Sort.cpp"
#include "src/Authentication.cpp"
#include "src/Logger.cpp"
#include "src/Trip.cpp"
#include "src/Navigator.cpp"
#include "src/Trips.cpp"
#include "src/Places.cpp"
#include "src/NavigatorAdmin.cpp"
#include "src/Utils.cpp"   // these two last: they bring in <windows.h>
#include "src/Style.cpp"
#endif
