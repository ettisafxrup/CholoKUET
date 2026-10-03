#pragma once

#include <string>

// Appends lines like
//   [2026-09-29 08:20:10] LOGIN user=student1 result=success
// to logs/application.log. Passwords are never passed in here.
class Logger
{
public:
    void open(const std::string& logPath) { path = logPath; }
    void log(const std::string& action, const std::string& details = "") const;
    void printLast(int lines) const;

private:
    std::string path;
};
