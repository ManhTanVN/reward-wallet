#include "data_manager.h"
#include "sha256.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

// ---------------------- Constructor & Filesystem -----------------------

DataManager::DataManager(const std::string &dataPath)
    : dataPath_(dataPath)
{
    ensureDataDirectoryExists();
}

std::string DataManager::getDataDirectory() const
{
    size_t slash = dataPath_.find_last_of("/\\");
    if (slash == std::string::npos)
        return "";
    return dataPath_.substr(0, slash);
}

void DataManager::ensureDataDirectoryExists() const
{
    std::string dir = getDataDirectory();
    if (!dir.empty())
    {
#ifdef _WIN32
        _mkdir(dir.c_str());
#else
        mkdir(dir.c_str(), 0755);
#endif
    }
}

void DataManager::writeJsonToFile(const json &j) const
{
    std::ofstream file(dataPath_);
    if (!file)
    {
        throw std::runtime_error("Cannot open file for writing: " + dataPath_);
    }
    file << std::setw(4) << j << std::endl;
}

json DataManager::readJsonFromFile() const
{
    std::ifstream file(dataPath_);
    if (!file)
        return json::array();
    json j;
    file >> j;
    return j;
}

// ---------------------- Cache Management -----------------------

void DataManager::loadCache()
{
    if (cacheLoaded_)
        return;

    auto users = loadAllUsers();
    for (const auto &user : users)
    {
        userMap_[user->getUsername()] = user;
    }
    cacheLoaded_ = true;
}

void DataManager::clearCache()
{
    userMap_.clear();
    cacheLoaded_ = false;
}

// ---------------------- User Operations -----------------------

void DataManager::saveUser(const std::shared_ptr<UserAccount> &user)
{
    loadCache();
    userMap_[user->getUsername()] = user;

    std::vector<std::shared_ptr<UserAccount>> users;
    for (const auto &[username, u] : userMap_)
    {
        users.push_back(u);
    }
    saveAllUsers(users);
}

void DataManager::removeUser(const std::string &username)
{
    loadCache();
    userMap_.erase(username);

    std::vector<std::shared_ptr<UserAccount>> users;
    for (const auto &[username, u] : userMap_)
    {
        users.push_back(u);
    }
    saveAllUsers(users);
}

std::shared_ptr<UserAccount> DataManager::findUser(const std::string &username)
{
    loadCache();
    auto it = userMap_.find(username);
    if (it != userMap_.end())
        return it->second;
    return nullptr;
}

std::vector<std::shared_ptr<UserAccount>> DataManager::loadAllUsers()
{
    json jArr = readJsonFromFile();
    std::vector<std::shared_ptr<UserAccount>> users;
    for (const auto &j : jArr)
    {
        users.push_back(jsonToUser(j));
    }
    return users;
}

void DataManager::saveAllUsers(const std::vector<std::shared_ptr<UserAccount>> &users)
{
    json jArr = json::array();
    for (const auto &user : users)
    {
        jArr.push_back(userToJson(user));
    }
    writeJsonToFile(jArr);
}

// ---------------------- Password Hashing -----------------------

std::string DataManager::hashPassword(const std::string &password)
{
    return sha256(password);
}

bool DataManager::verifyPassword(const std::string &password, const std::string &hash)
{
    return hashPassword(password) == hash;
}

// ---------------------- JSON Conversion -----------------------

json DataManager::userToJson(const std::shared_ptr<UserAccount> &user) const
{
    return {
        {"fullName", user->getFullName()},
        {"email", user->getEmail()},
        {"idNumber", user->getIdNumber()},
        {"username", user->getUsername()},
        {"hashedPassword", user->getHashedPassword()},
        {"status", static_cast<int>(user->getStatus())},
        {"isTempPassword", user->isUsingTempPassword()},
        {"creationDate", std::chrono::duration_cast<std::chrono::seconds>(
                             user->getCreationDate().time_since_epoch())
                             .count()}};
}

std::shared_ptr<UserAccount> DataManager::jsonToUser(const json &j) const
{
    auto user = std::make_shared<UserAccount>(
        j.at("fullName"),
        j.at("email"),
        j.at("idNumber"),
        j.at("username"),
        "");
    user->setHashedPassword(j.at("hashedPassword"));
    user->setStatus(static_cast<AccountStatus>(j.at("status")));
    user->setTempPassword(j.at("isTempPassword"));
    std::chrono::system_clock::time_point creation =
        std::chrono::system_clock::time_point(std::chrono::seconds(j.at("creationDate")));
    user->setCreationDate(creation);
    return user;
}
