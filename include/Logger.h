#pragma once

#include <string>

class Logger
{
public:
    void open(const std::string &logPath) { path = logPath; }
    void log(const std::string &action, const std::string &details = "") const;
    void printLast(int lines) const;

private:
    std::string path;
};
