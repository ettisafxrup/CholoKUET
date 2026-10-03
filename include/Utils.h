#pragma once

#include <string>
#include <vector>

// ---- Strings ----

std::string trim(const std::string& text);
std::string toLower(std::string text);
int compareIgnoreCase(const std::string& a, const std::string& b);
bool equalsIgnoreCase(const std::string& a, const std::string& b);
bool containsIgnoreCase(const std::string& text, const std::string& part);
std::vector<std::string> split(const std::string& line, char separator);

// Strict parsing: the whole string must be a number.
bool parseInt(const std::string& text, int& value);
bool parseDouble(const std::string& text, double& value);

// "22.8992, 89.5016" or "22.8992 89.5016" -> latitude and longitude.
bool parseCoordinates(const std::string& text, double& latitude, double& longitude);

// ---- Console input ----
// None of these crash on bad input; they ask again instead. Once stdin is
// closed (e.g. the end of a piped script) they return 0 / "" / false, so
// every menu falls back to "Back" and the program exits cleanly.

std::string readLine(const std::string& prompt);
double readDouble(const std::string& prompt);

// Menu choice from 0 to highest. With enterMeansBack, pressing Enter gives 0.
int readChoice(int highest, bool enterMeansBack = true);

// y / n question; pressing Enter picks defaultAnswer.
bool readYesNo(const std::string& question, bool defaultAnswer);

std::string readPassword(const std::string& prompt);
bool inputClosed();
void pause();

// ---- Console output ----

void initConsole();   // UTF-8 output and colours
void printTitle(const std::string& title, const std::string& subtitle = "");
void printRule(int width = 58);
void printOption(int number, const std::string& label);   // "   1  Label" in a menu
void printSuccess(const std::string& message);
void printWarning(const std::string& message);
void printError(const std::string& message);

// ---- System ----

bool copyToClipboard(const std::string& text);
bool fileExists(const std::string& path);
void makeDirectory(const std::string& path);
std::string currentTimestamp();
