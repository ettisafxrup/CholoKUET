#include "../include/Authentication.h"
#include "../include/Config.h"
#include "../include/Utils.h"

#include <cctype>
#include <cstdio>
#include <fstream>

std::string Authentication::hashPassword(const std::string& username, const std::string& password)
{
    unsigned long long hash = 14695981039346656037ULL;   // FNV-1a 64-bit offset basis
    for (char c : toLower(username) + ":" + password)
    {
        hash ^= static_cast<unsigned char>(c);
        hash *= 1099511628211ULL;                          // FNV prime
    }

    char hex[17];
    std::snprintf(hex, sizeof(hex), "%016llx", hash);
    return hex;
}

bool Authentication::validUsername(const std::string& username, std::string& error)
{
    if (username.size() < 3 || username.size() > 30)
    {
        error = "Username must be 3 to 30 characters long.";
        return false;
    }
    for (char c : username)
    {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '.')
        {
            error = "Username can only use letters, digits, '_' and '.'.";
            return false;
        }
    }
    return true;
}

bool Authentication::validPassword(const std::string& password, std::string& error)
{
    if (password.size() < 4)
    {
        error = "Password must be at least 4 characters.";
        return false;
    }
    if (password.find('|') != std::string::npos)
    {
        error = "Password can't contain '|'.";
        return false;
    }
    return true;
}

int Authentication::findUser(const std::string& username) const
{
    for (size_t i = 0; i < users.size(); i++)
    {
        if (equalsIgnoreCase(users[i].username, username))
            return static_cast<int>(i);
    }
    return -1;
}

bool Authentication::load(const std::string& path)
{
    std::ifstream file(path);
    if (!file)
        return false;

    users.clear();
    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> fields = split(line, '|');
        std::string error;
        if (fields.size() < 3 || !validUsername(fields[0], error) || fields[1].size() != 16 ||
            findUser(fields[0]) != -1 || users.size() >= static_cast<size_t>(MAX_USERS))
            continue;

        users.push_back({fields[0], fields[1], fields[2] == "admin" ? "admin" : "student"});
    }
    return true;
}

bool Authentication::save(const std::string& path) const
{
    std::ofstream file(path);
    if (!file)
        return false;

    file << "# username|passwordHash|role\n"
         << "# The hash is FNV-1a of \"username:password\". Fine for a lab demo,\n"
         << "# not secure for real use (that needs Argon2, bcrypt or scrypt).\n";
    for (const User& user : users)
        file << user.username << "|" << user.passwordHash << "|" << user.role << "\n";
    return static_cast<bool>(file);
}

void Authentication::addDefaultUsers()
{
    std::string error;
    registerUser("admin", "admin123", "admin", error);
    registerUser("student1", "student123", "student", error);
}

bool Authentication::registerUser(const std::string& username, const std::string& password,
                                  const std::string& role, std::string& error)
{
    if (!validUsername(username, error) || !validPassword(password, error))
        return false;

    if (findUser(username) != -1)
    {
        error = "That username is already taken.";
        return false;
    }
    if (users.size() >= static_cast<size_t>(MAX_USERS))
    {
        error = "The user list is full.";
        return false;
    }

    users.push_back({username, hashPassword(username, password), role == "admin" ? "admin" : "student"});
    return true;
}

bool Authentication::login(const std::string& username, const std::string& password)
{
    int index = findUser(username);
    if (index == -1 || users[index].passwordHash != hashPassword(users[index].username, password))
        return false;

    currentIndex = index;
    return true;
}
