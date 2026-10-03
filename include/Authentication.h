#pragma once

#include <string>
#include <vector>

// Simple offline login for the lab project.
//
// users.txt stores "username|hash|role". The hash is FNV-1a over
// "username:password", so the file never contains plain passwords. It is
// NOT real password security (FNV is fast and easy to brute force); a real
// system would use Argon2, bcrypt or scrypt.

struct User
{
    std::string username;
    std::string passwordHash;
    std::string role;   // "student" or "admin"
};

class Authentication
{
public:
    static std::string hashPassword(const std::string& username, const std::string& password);
    static bool validUsername(const std::string& username, std::string& error);
    static bool validPassword(const std::string& password, std::string& error);

    bool load(const std::string& path);   // false if the file is missing
    bool save(const std::string& path) const;
    void addDefaultUsers();               // admin/admin123 and student1/student123

    bool registerUser(const std::string& username, const std::string& password,
                      const std::string& role, std::string& error);
    bool login(const std::string& username, const std::string& password);
    void logout() { currentIndex = -1; }

    bool loggedIn() const { return currentIndex != -1; }
    bool isAdmin() const { return loggedIn() && users[currentIndex].role == "admin"; }
    const User& currentUser() const { return users[currentIndex]; }
    const std::vector<User>& allUsers() const { return users; }

private:
    std::vector<User> users;
    int currentIndex = -1;

    int findUser(const std::string& username) const;
};
