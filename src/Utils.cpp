#include "../include/Utils.h"
#include "../include/Style.h"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <conio.h>
    #include <direct.h>
    #include <io.h>
#else
    #include <sys/stat.h>
#endif

static bool stdinClosed = false;

// ---- Strings ----

std::string trim(const std::string& text)
{
    size_t start = 0;
    size_t end = text.size();
    while (start < end && std::isspace(static_cast<unsigned char>(text[start])))
        start++;
    while (end > start && std::isspace(static_cast<unsigned char>(text[end - 1])))
        end--;
    return text.substr(start, end - start);
}

std::string toLower(std::string text)
{
    for (char& c : text)
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return text;
}

int compareIgnoreCase(const std::string& a, const std::string& b)
{
    return toLower(a).compare(toLower(b));
}

bool equalsIgnoreCase(const std::string& a, const std::string& b)
{
    return compareIgnoreCase(a, b) == 0;
}

bool containsIgnoreCase(const std::string& text, const std::string& part)
{
    return toLower(text).find(toLower(part)) != std::string::npos;
}

std::vector<std::string> split(const std::string& line, char separator)
{
    std::vector<std::string> fields;
    size_t start = 0;
    while (true)
    {
        size_t pos = line.find(separator, start);
        if (pos == std::string::npos)
        {
            fields.push_back(trim(line.substr(start)));
            return fields;
        }
        fields.push_back(trim(line.substr(start, pos - start)));
        start = pos + 1;
    }
}

bool parseInt(const std::string& text, int& value)
{
    std::string s = trim(text);
    if (s.empty())
        return false;

    char* end = nullptr;
    long parsed = std::strtol(s.c_str(), &end, 10);
    if (*end != '\0' || parsed < -2147483647L || parsed > 2147483647L)
        return false;

    value = static_cast<int>(parsed);
    return true;
}

bool parseDouble(const std::string& text, double& value)
{
    std::string s = trim(text);
    if (s.empty())
        return false;

    char* end = nullptr;
    double parsed = std::strtod(s.c_str(), &end);
    if (*end != '\0' || !std::isfinite(parsed))
        return false;

    value = parsed;
    return true;
}

bool parseCoordinates(const std::string& text, double& latitude, double& longitude)
{
    // Accepts "22.8992, 89.5016" or "22.8992 89.5016".
    std::string cleaned = text;
    for (char& c : cleaned)
    {
        if (c == ',')
            c = ' ';
    }

    char* end = nullptr;
    const char* start = cleaned.c_str();
    double lat = std::strtod(start, &end);
    if (end == start)
        return false;

    const char* second = end;
    double lon = std::strtod(second, &end);
    if (end == second || !trim(end).empty() || !std::isfinite(lat) || !std::isfinite(lon))
        return false;

    latitude = lat;
    longitude = lon;
    return true;
}

// ---- Console input ----

std::string readLine(const std::string& prompt)
{
    std::cout << prompt;
    std::string line;
    if (stdinClosed || !std::getline(std::cin, line))
    {
        stdinClosed = true;
        std::cout << "\n";
        return "";
    }
    return trim(line);
}

int readChoice(int highest, bool enterMeansBack)
{
    while (true)
    {
        std::cout << "\n  " << style::accent << "›" << style::reset << " ";
        std::string line = readLine("");
        if (stdinClosed || (line.empty() && enterMeansBack))
            return 0;

        int value;
        if (parseInt(line, value) && value >= 0 && value <= highest)
            return value;
        if (!line.empty())
            std::cout << style::warn << "  Choose a number from the list (0 - " << highest << ")."
                      << style::reset << "\n";
    }
}

double readDouble(const std::string& prompt)
{
    while (true)
    {
        std::string line = readLine(prompt);
        if (stdinClosed)
            return 0.0;

        double value;
        if (parseDouble(line, value))
            return value;
        std::cout << style::warn << "  Please type a number, e.g. 22.899300" << style::reset << "\n";
    }
}

bool readYesNo(const std::string& question, bool defaultAnswer)
{
    std::string prompt = question + (defaultAnswer ? " [Y/n] " : " [y/N] ");
    while (true)
    {
        std::string answer = toLower(readLine(prompt));
        if (stdinClosed)
            return false;
        if (answer.empty())
            return defaultAnswer;
        if (answer == "y" || answer == "yes")
            return true;
        if (answer == "n" || answer == "no")
            return false;
        std::cout << style::warn << "  Please answer y or n." << style::reset << "\n";
    }
}

std::string readPassword(const std::string& prompt)
{
#ifdef _WIN32
    // Hide what is typed on a real console. When input is piped in (tests,
    // scripted demos) there is no console, so fall back to a normal read.
    if (!stdinClosed && _isatty(_fileno(stdin)))
    {
        std::cout << prompt << std::flush;
        std::string password;
        while (true)
        {
            int ch = _getch();
            if (ch == '\r' || ch == '\n')
                break;
            if (ch == 0 || ch == 0xE0)
            {
                _getch();   // arrow keys etc. send two codes; ignore both
                continue;
            }
            if (ch == '\b')
            {
                if (!password.empty())
                {
                    password.pop_back();
                    std::cout << "\b \b";
                }
                continue;
            }
            if (ch >= 32 && ch < 127)
            {
                password += static_cast<char>(ch);
                std::cout << '*';
            }
        }
        std::cout << "\n";
        return password;
    }
#endif
    return readLine(prompt);
}

bool inputClosed()
{
    return stdinClosed;
}

void pause()
{
    if (stdinClosed)
        return;
    std::cout << style::muted;
    readLine("\n  Press Enter to go back...");
    std::cout << style::reset;
}

// ---- Console output ----

void initConsole()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   // so the box and arrow characters render
#endif
    style::init();
}

void printRule(int width)
{
    std::cout << style::muted << "  ";
    for (int i = 0; i < width; i++)
        std::cout << "─";
    std::cout << style::reset << "\n";
}

void printTitle(const std::string& title, const std::string& subtitle)
{
    std::cout << "\n  " << style::heading << title << style::reset << "\n";
    if (!subtitle.empty())
        std::cout << "  " << style::muted << subtitle << style::reset << "\n";
    printRule();
}

void printOption(int number, const std::string& label)
{
    std::cout << "   " << style::accent << number << style::reset << "  " << label << "\n";
}

void printSuccess(const std::string& message)
{
    std::cout << style::good << "  ✓ " << message << style::reset << "\n";
}

void printWarning(const std::string& message)
{
    std::cout << style::warn << "  ! " << message << style::reset << "\n";
}

void printError(const std::string& message)
{
    std::cout << style::bad << "  ✗ " << message << style::reset << "\n";
}

// ---- System ----

bool copyToClipboard(const std::string& text)
{
#ifdef _WIN32
    if (!OpenClipboard(nullptr))
        return false;
    EmptyClipboard();

    HGLOBAL memory = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!memory)
    {
        CloseClipboard();
        return false;
    }
    char* buffer = static_cast<char*>(GlobalLock(memory));
    std::memcpy(buffer, text.c_str(), text.size() + 1);
    GlobalUnlock(memory);

    bool ok = SetClipboardData(CF_TEXT, memory) != nullptr;
    if (!ok)
        GlobalFree(memory);
    CloseClipboard();
    return ok;
#else
    (void)text;
    return false;
#endif
}

bool fileExists(const std::string& path)
{
    std::ifstream file(path);
    return file.good();
}

void makeDirectory(const std::string& path)
{
#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0755);
#endif
}

std::string currentTimestamp()
{
    std::time_t now = std::time(nullptr);
    char buffer[32] = "0000-00-00 00:00:00";
    if (std::tm* local = std::localtime(&now))
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", local);
    return buffer;
}
