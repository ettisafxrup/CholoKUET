#include "../include/Logger.h"
#include "../include/Utils.h"

#include <deque>
#include <fstream>
#include <iostream>

void Logger::log(const std::string &action, const std::string &details) const
{
    std::ofstream file(path, std::ios::app);
    if (!file)
        return;

    file << "[" << currentTimestamp() << "] " << action;
    if (!details.empty())
        file << " " << details;
    file << "\n";
}

void Logger::printLast(int lines) const
{
    std::ifstream file(path);
    if (!file)
    {
        std::cout << "  No log file yet.\n";
        return;
    }

    std::deque<std::string> recent;
    std::string line;
    int total = 0;
    while (std::getline(file, line))
    {
        recent.push_back(line);
        if (static_cast<int>(recent.size()) > lines)
            recent.pop_front();
        total++;
    }

    for (const std::string &entry : recent)
        std::cout << "  " << entry << "\n";
    std::cout << "\n  Showing the last " << recent.size() << " of " << total << " entries.\n";
}
