#pragma once

#include "user_account.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using json = nlohmann::json;

class DataManager
{
public:
    DataManager(const std::string &dataPath = "data/users.json");

    // Data storage operations
    void saveUser(const std::shared_ptr<UserAccount> &user);
    void saveAllUsers(const std::vector<std::shared_ptr<UserAccount>> &users);
    std::vector<std::shared_ptr<UserAccount>> loadAllUsers();
    void removeUser(const std::string &username);

    // Password hashing
    static std::string hashPassword(const std::string &password);
    static bool verifyPassword(const std::string &password, const std::string &hash);

private:
    fs::path dataPath_;
    fs::path getDataDirectory() const;
    void ensureDataDirectoryExists() const;
    void writeJsonToFile(const json &j) const;
    json readJsonFromFile() const;

    // JSON conversion
    json userToJson(const std::shared_ptr<UserAccount> &user) const;
    std::shared_ptr<UserAccount> jsonToUser(const json &j) const;
};