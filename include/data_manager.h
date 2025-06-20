#pragma once

#include "user_account.h"
#include <nlohmann/json.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

#if defined(_WIN32)
#include <direct.h>
#define mkdir _mkdir
#else
#include <sys/stat.h>
#endif

using json = nlohmann::json;

class DataManager
{
public:
    DataManager(const std::string &dataPath = "data/users.json");

    // User account operations
    void saveUser(const std::shared_ptr<UserAccount> &user);
    void saveAllUsers(const std::vector<std::shared_ptr<UserAccount>> &users);
    std::vector<std::shared_ptr<UserAccount>> loadAllUsers(); // load from file
    void removeUser(const std::string &username);
    std::shared_ptr<UserAccount> findUser(const std::string &username);

    // Password hashing
    static std::string hashPassword(const std::string &password);
    static bool verifyPassword(const std::string &password, const std::string &hash);

    // Optional: force reload data from file
    void clearCache();

private:
    std::string dataPath_;

    // Internal cache
    std::unordered_map<std::string, std::shared_ptr<UserAccount>> userMap_;
    bool cacheLoaded_ = false;

    // File operations
    std::string getDataDirectory() const;
    void ensureDataDirectoryExists() const;
    void writeJsonToFile(const json &j) const;
    json readJsonFromFile() const;

    // JSON conversion
    json userToJson(const std::shared_ptr<UserAccount> &user) const;
    std::shared_ptr<UserAccount> jsonToUser(const json &j) const;

    // Internal cache loading
    void loadCache();
};
