#include "../include/Style.h"

// termcolor on Windows defaults to the old console API, which can't do our
// gold. ANSI escape codes can, once virtual terminal mode is switched on.
#define TERMCOLOR_USE_ANSI_ESCAPE_SEQUENCES
#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
#endif
#include <cstdint>   // termcolor uses uint8_t without including this itself
#include <cstdlib>

#include "../third_party/termcolor/termcolor.hpp"

namespace
{
    bool colourOn = false;

    bool stdoutIsTerminal()
    {
#ifdef _WIN32
        return _isatty(_fileno(stdout));
#else
        return isatty(fileno(stdout));
#endif
    }

    bool enableAnsi()
    {
#ifdef _WIN32
        HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if (console == INVALID_HANDLE_VALUE || !GetConsoleMode(console, &mode))
            return false;
        return SetConsoleMode(console, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
#else
        return true;
#endif
    }
}

namespace style
{
    void init()
    {
        // NO_COLOR is a common convention for "please don't colour output".
        colourOn = stdoutIsTerminal() && enableAnsi() && std::getenv("NO_COLOR") == nullptr;
    }

    bool enabled()
    {
        return colourOn;
    }

    std::ostream& accent(std::ostream& os)
    {
        return colourOn ? termcolor::color<217, 177, 0>(os) : os;
    }

    std::ostream& heading(std::ostream& os)
    {
        return colourOn ? termcolor::color<217, 177, 0>(termcolor::bold(os)) : os;
    }

    std::ostream& strong(std::ostream& os)
    {
        return colourOn ? termcolor::bold(os) : os;
    }

    std::ostream& muted(std::ostream& os)
    {
        return colourOn ? termcolor::color<140, 142, 150>(os) : os;
    }

    std::ostream& good(std::ostream& os)
    {
        return colourOn ? termcolor::color<95, 190, 120>(os) : os;
    }

    std::ostream& warn(std::ostream& os)
    {
        return colourOn ? termcolor::color<235, 150, 60>(os) : os;
    }

    std::ostream& bad(std::ostream& os)
    {
        return colourOn ? termcolor::color<230, 85, 85>(os) : os;
    }

    std::ostream& reset(std::ostream& os)
    {
        return colourOn ? termcolor::reset(os) : os;
    }

    void clearScreen()
    {
        if (colourOn)
            std::cout << "\033[2J\033[H" << std::flush;
    }
}
