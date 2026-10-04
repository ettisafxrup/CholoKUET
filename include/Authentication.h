#pragma once

#include <string>
#include <vector>

struct User
{
    std::string username;
    std::string passwordHash;
    std::string role;
};

class Authentication
{
public:
    static std::string hashPassword(const std::string &username, const std::string &password);
    static bool validUsername(const std::string &username, std::string &error);
    static bool validPassword(const std::string &password, std::string &error);

    bool load(const std::string &path);
    bool save(const std::string &path) const;
    void addDefaultUsers();

    bool registerUser(const std::string &username, const std::string &password,
                      const std::string &role, std::string &error);
    bool login(const std::string &username, const std::string &password);
    void logout() { currentIndex = -1; }

    bool loggedIn() const { return currentIndex != -1; }
    bool isAdmin() const { return loggedIn() && users[currentIndex].role == "admin"; }
    const User &currentUser() const { return users[currentIndex]; }
    const std::vector<User> &allUsers() const { return users; }

private:
    std::vector<User> users;
    int currentIndex = -1;

    int findUser(const std::string &username) const;
};
