#pragma once

#include <iostream>

// Terminal colours, built on the termcolor library (third_party/termcolor).
//
// Use them like std::endl:
//     std::cout << style::accent << "CholoKUET" << style::reset;
//
// Colours are only written when output goes to a real terminal, so piping
// the program into a file or a test gives plain text.
namespace style
{
    // Turns on colour support. Call once at startup.
    void init();
    bool enabled();

    std::ostream& accent(std::ostream& os);    // KUET gold, #d9b100
    std::ostream& heading(std::ostream& os);   // bold gold, for titles
    std::ostream& strong(std::ostream& os);    // bold, for names and values
    std::ostream& muted(std::ostream& os);     // grey, for hints and details
    std::ostream& good(std::ostream& os);      // green, for success
    std::ostream& warn(std::ostream& os);      // amber, for warnings
    std::ostream& bad(std::ostream& os);       // red, for errors
    std::ostream& reset(std::ostream& os);

    // Clears the screen on a real terminal; does nothing otherwise.
    void clearScreen();
}
