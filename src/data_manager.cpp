#include "data_manager.h"
#include "utils.h"
#include "sha256.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <random>

// ---------------------- Constructor & Filesystem -----------------------

// Khởi tạo DataManager với đường dẫn đến file dữ liệu JSON
DataManager::DataManager(const std::string &dataPath)
    : dataPath_(dataPath)
{
    ensureDataDirectoryExists();
}

// Lấy đường dẫn thư mục chứa file dữ liệu
std::string DataManager::getDataDirectory() const
{
    size_t slash = dataPath_.find_last_of("/\\");
    if (slash == std::string::npos)
        return "";
    return dataPath_.substr(0, slash);
}

// Đảm bảo thư mục dữ liệu tồn tại, tạo nếu chưa có
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

// Ghi đối tượng JSON vào file dữ liệu
void DataManager::writeJsonToFile(const json &j) const
{
    std::ofstream file(dataPath_);
    if (!file)
        throw std::runtime_error("Cannot open file for writing: " + dataPath_);
    file << std::setw(4) << j << std::endl;
}

// Đọc và trả về nội dung file JSON, nếu không có thì trả về mảng rỗng
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

// Tải toàn bộ dữ liệu người dùng vào cache để tăng hiệu suất truy xuất
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

// Xóa cache
void DataManager::clearCache()
{
    userMap_.clear();
    cacheLoaded_ = false;
}

// ---------------------- User Operations -----------------------

// Lưu thông tin người dùng, cập nhật lại file dữ liệu và cache
void DataManager::saveUser(const std::shared_ptr<UserAccount> &user, bool makeBackup)
{
    loadCache();
    userMap_[user->getUsername()] = user;

    std::vector<std::shared_ptr<UserAccount>> users;
    for (const auto &[username, u] : userMap_)
    {
        users.push_back(u);
    }

    saveAllUsers(users);

    if (makeBackup)
    {
        createBackup("data/users.json", "backups");
    }
}

// Xóa người dùng khỏi cache và file lưu trữ
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
    createBackup("data/users.json", "backups");
}

// Tìm người dùng theo tên đăng nhập
std::shared_ptr<UserAccount> DataManager::findUser(const std::string &username)
{
    loadCache();
    auto it = userMap_.find(username);
    if (it != userMap_.end())
        return it->second;
    return nullptr;
}

// Tìm người dùng theo địa chỉ ví
std::shared_ptr<UserAccount> DataManager::findUserByWallet(const std::string &walletAddr)
{
    auto users = loadAllUsers();
    for (const auto &user : users)
    {
        if (user->getWalletAddress() == walletAddr)
        {
            return user;
        }
    }
    return nullptr;
}

// Tải toàn bộ danh sách người dùng từ file
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

// Ghi toàn bộ danh sách người dùng vào file
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

// Mã hóa mật khẩu bằng hàm băm SHA-256
std::string DataManager::hashPassword(const std::string &password)
{
    return sha256(password);
}

// So sánh mật khẩu với chuỗi băm đã lưu
bool DataManager::verifyPassword(const std::string &password, const std::string &hash)
{
    return hashPassword(password) == hash;
}

// ---------------------- JSON Conversion -----------------------

// Chuyển đối tượng UserAccount thành JSON để lưu trữ
json DataManager::userToJson(const std::shared_ptr<UserAccount> &user) const
{
    return {
        {"fullName", user->getFullName()},
        {"email", user->getEmail()},
        {"username", user->getUsername()},
        {"hashedPassword", user->getHashedPassword()},
        {"status", static_cast<int>(user->getStatus())},
        {"isTempPassword", user->isUsingTempPassword()},
        {"creationDate", std::chrono::duration_cast<std::chrono::seconds>(
                             user->getCreationDate().time_since_epoch())
                             .count()},
        {"role", static_cast<int>(user->getRole())},
        {"walletAddress", user->getWalletAddress()},
        {"pointBalance", user->getPointBalance()},
        {"transactionHistory", user->getTransactionHistory()}};
}

// Chuyển đối tượng JSON thành UserAccount
std::shared_ptr<UserAccount> DataManager::jsonToUser(const json &j) const
{
    auto user = std::make_shared<UserAccount>(
        j.at("fullName"),
        j.at("email"),
        j.at("username"),
        "" // Mật khẩu không cần thiết, sẽ set hash ở dưới
    );

    user->setHashedPassword(j.at("hashedPassword"));
    user->setStatus(static_cast<AccountStatus>(j.at("status")));
    user->setTempPassword(j.at("isTempPassword"));
    std::chrono::system_clock::time_point creation =
        std::chrono::system_clock::time_point(std::chrono::seconds(j.at("creationDate")));
    user->setCreationDate(creation);

    if (j.contains("role"))
        user->setRole(static_cast<UserRole>(j.at("role")));
    if (j.contains("walletAddress"))
        user->setWalletAddress(j.at("walletAddress"));
    if (j.contains("pointBalance"))
        user->setPointBalance(j.at("pointBalance"));
    if (j.contains("transactionHistory"))
        user->setTransactionHistory(j.at("transactionHistory").get<std::vector<std::string>>());

    return user;
}
// ---------------------- Backup Management -----------------------