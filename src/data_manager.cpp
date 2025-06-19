#include "data_manager.h"
#include <fstream>
#include <stdexcept>

DataManager::DataManager(const std::string &dataPath)
    : dataPath_(dataPath)
{
    ensureDataDirectoryExists();
}

void DataManager::saveUser(const std::shared_ptr<UserAccount> &user)
{
    json usersJson;

    if (fs::exists(dataPath_))
    {
        usersJson = readJsonFromFile();
    }

    usersJson["users"][user->getUsername()] = userToJson(user);
    writeJsonToFile(usersJson);
}

void DataManager::saveAllUsers(const std::vector<std::shared_ptr<UserAccount>> &users)
{
    json usersJson;
    usersJson["users"] = json::object();

    for (const auto &user : users)
    {
        usersJson["users"][user->getUsername()] = userToJson(user);
    }

    writeJsonToFile(usersJson);
}

std::vector<std::shared_ptr<UserAccount>> DataManager::loadAllUsers()
{
    std::vector<std::shared_ptr<UserAccount>> users;

    if (!fs::exists(dataPath_))
    {
        return users;
    }

    json usersJson = readJsonFromFile();

    for (const auto &[username, userJson] : usersJson["users"].items())
    {
        users.push_back(jsonToUser(userJson));
    }

    return users;
}

void DataManager::removeUser(const std::string &username)
{
    if (!fs::exists(dataPath_))
    {
        return;
    }

    json usersJson = readJsonFromFile();
    usersJson["users"].erase(username);
    writeJsonToFile(usersJson);
}

std::string DataManager::hashPassword(const std::string &password)
{
    return BCrypt::generateHash(password);
}

bool DataManager::verifyPassword(const std::string &password, const std::string &hash)
{
    return BCrypt::validatePassword(password, hash);
}

fs::path DataManager::getDataDirectory() const
{
    return dataPath_.parent_path();
}

void DataManager::ensureDataDirectoryExists() const
{
    fs::create_directories(getDataDirectory());
}

void DataManager::writeJsonToFile(const json &j) const
{
    std::ofstream file(dataPath_);
    if (!file)
    {
        throw std::runtime_error("Could not open file for writing: " + dataPath_.string());
    }
    file << std::setw(4) << j << std::endl;
}

json DataManager::readJsonFromFile() const
{
    std::ifstream file(dataPath_);
    if (!file)
    {
        throw std::runtime_error("Could not open file for reading: " + dataPath_.string());
    }
    json j;
    file >> j;
    return j;
}

json DataManager::userToJson(const std::shared_ptr<UserAccount> &user) const
{
    json j;
    j["fullName"] = user->getFullName();
    j["email"] = user->getEmail();
    j["idNumber"] = user->getIdNumber();
    j["username"] = user->getUsername();
    j["hashedPassword"] = user->getHashedPassword();
    j["status"] = static_cast<int>(user->getStatus());
    j["isTempPassword"] = user->isTempPassword();
    j["creationDate"] = std::chrono::system_clock::to_time_t(user->getCreationDate());
    return j;
}

std::shared_ptr<UserAccount> DataManager::jsonToUser(const json &j) const
{
    auto user = std::make_shared<UserAccount>(
        j["fullName"].get<std::string>(),
        j["email"].get<std::string>(),
        j["idNumber"].get<std::string>(),
        j["username"].get<std::string>(),
        "" // Password is already hashed
    );

    user->setHashedPassword(j["hashedPassword"].get<std::string>());
    user->setStatus(static_cast<AccountStatus>(j["status"].get<int>()));
    user->setTempPassword(j["isTempPassword"].get<bool>());

    return user;
}