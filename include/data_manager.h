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
#define mkdir _mkdir // Định nghĩa lại mkdir cho Windows
#else
#include <sys/stat.h> // Thư viện tạo thư mục trên Unix
#endif

using json = nlohmann::json;

/**
 * @class DataManager
 * @brief Quản lý dữ liệu người dùng: lưu trữ, truy xuất, mã hóa mật khẩu, chuyển đổi JSON.
 */
class DataManager
{
public:
    /**
     * @brief Constructor khởi tạo DataManager với đường dẫn file dữ liệu người dùng.
     * @param dataPath Đường dẫn tới file JSON chứa dữ liệu người dùng.
     */
    DataManager(const std::string &dataPath = "data/users.json");

    // ================== Quản lý tài khoản người dùng ==================

    /**
     * @brief Lưu thông tin một người dùng (ghi đè nếu đã tồn tại).
     * @param user Đối tượng người dùng cần lưu.
     * @param makeBackup Tự động tạo bản sao lưu sau khi lưu (mặc định: true).
     */
    void saveUser(const std::shared_ptr<UserAccount> &user, bool makeBackup = true);

    /**
     * @brief Lưu toàn bộ danh sách người dùng vào file.
     * @param users Danh sách người dùng.
     */
    void saveAllUsers(const std::vector<std::shared_ptr<UserAccount>> &users);

    /**
     * @brief Tải toàn bộ người dùng từ file (không cache).
     * @return Danh sách con trỏ tới UserAccount.
     */
    std::vector<std::shared_ptr<UserAccount>> loadAllUsers();

    /**
     * @brief Xóa người dùng theo username.
     * @param username Tên người dùng cần xóa.
     */
    void removeUser(const std::string &username);

    /**
     * @brief Tìm người dùng theo username từ cache hoặc file.
     * @param username Tên người dùng.
     * @return Con trỏ tới UserAccount nếu tìm thấy, nullptr nếu không.
     */
    std::shared_ptr<UserAccount> findUser(const std::string &username);

    /**
     * @brief Tìm người dùng theo địa chỉ ví.
     * @param walletAddr Địa chỉ ví cần tìm.
     * @return Con trỏ tới UserAccount nếu tìm thấy.
     */
    std::shared_ptr<UserAccount> findUserByWallet(const std::string &walletAddr);

    // ================== Mã hóa mật khẩu ==================

    /**
     * @brief Sinh chuỗi băm (SHA-256) từ mật khẩu thô.
     * @param password Chuỗi mật khẩu ban đầu.
     * @return Chuỗi đã mã hóa.
     */
    static std::string hashPassword(const std::string &password);

    /**
     * @brief So sánh mật khẩu nhập vào với mật khẩu đã băm.
     * @param password Mật khẩu người dùng nhập vào.
     * @param hash Mật khẩu đã băm lưu trong hệ thống.
     * @return true nếu khớp, false nếu không.
     */
    static bool verifyPassword(const std::string &password, const std::string &hash);

    // ================== Quản lý bộ nhớ đệm ==================

    /**
     * @brief Xóa cache nội bộ, buộc tải lại dữ liệu từ file lần sau.
     */
    void clearCache();

private:
    // ================== Thuộc tính nội bộ ==================
    std::string dataPath_; ///< Đường dẫn đến file JSON chứa danh sách người dùng.

    std::unordered_map<std::string, std::shared_ptr<UserAccount>> userMap_; ///< Bản đồ cache người dùng theo username.
    bool cacheLoaded_ = false;                                              ///< Đánh dấu cache đã được tải chưa.

    // ================== Thao tác file ==================

    /**
     * @brief Trích xuất đường dẫn thư mục từ đường dẫn file.
     * @return Đường dẫn thư mục chứa file.
     */
    std::string getDataDirectory() const;

    /**
     * @brief Tạo thư mục chứa file nếu chưa tồn tại.
     */
    void ensureDataDirectoryExists() const;

    /**
     * @brief Ghi đối tượng JSON vào file `dataPath_`.
     * @param j Đối tượng JSON cần ghi.
     */
    void writeJsonToFile(const json &j) const;

    /**
     * @brief Đọc dữ liệu từ file JSON.
     * @return Mảng JSON đọc được từ file.
     */
    json readJsonFromFile() const;

    // ================== Chuyển đổi JSON <-> UserAccount ==================

    /**
     * @brief Chuyển UserAccount sang JSON để lưu vào file.
     * @param user Con trỏ tới đối tượng UserAccount.
     * @return Đối tượng JSON biểu diễn người dùng.
     */
    json userToJson(const std::shared_ptr<UserAccount> &user) const;

    /**
     * @brief Phục hồi đối tượng UserAccount từ JSON.
     * @param j Đối tượng JSON đại diện người dùng.
     * @return Con trỏ tới UserAccount tương ứng.
     */
    std::shared_ptr<UserAccount> jsonToUser(const json &j) const;

    // ================== Cache loader ==================

    /**
     * @brief Nạp toàn bộ user từ file vào cache (userMap_).
     */
    void loadCache();
};
