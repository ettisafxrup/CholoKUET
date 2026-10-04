#pragma once

#include <string>
#include <vector>

std::string trim(const std::string &text);
std::string toLower(std::string text);
int compareIgnoreCase(const std::string &a, const std::string &b);
bool equalsIgnoreCase(const std::string &a, const std::string &b);
bool containsIgnoreCase(const std::string &text, const std::string &part);
std::vector<std::string> split(const std::string &line, char separator);

bool parseInt(const std::string &text, int &value);
bool parseDouble(const std::string &text, double &value);

bool parseCoordinates(const std::string &text, double &latitude, double &longitude);

std::string readLine(const std::string &prompt);
double readDouble(const std::string &prompt);
int readChoice(int highest, bool enterMeansBack = true);

bool readYesNo(const std::string &question, bool defaultAnswer);

std::string readPassword(const std::string &prompt);
bool inputClosed();
void pause();

void initConsole();
void printTitle(const std::string &title, const std::string &subtitle = "");
void printRule(int width = 58);
void printOption(int number, const std::string &label);
void printSuccess(const std::string &message);
void printWarning(const std::string &message);
void printError(const std::string &message);

bool copyToClipboard(const std::string &text);
bool fileExists(const std::string &path);
void makeDirectory(const std::string &path);
std::string currentTimestamp();
