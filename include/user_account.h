#pragma once // Đảm bảo file header chỉ được include một lần trong quá trình biên dịch

#include <string>
#include <vector>
#include <chrono>

// Liệt kê các trạng thái tài khoản
enum class AccountStatus
{
    ACTIVE,    // Đang hoạt động
    SUSPENDED, // Bị tạm khóa
    PENDING,   // Chờ xác thực
    INACTIVE   // Không hoạt động
};

// Liệt kê các vai trò người dùng
enum class UserRole
{
    ADMIN, // Quản trị viên
    USER   // Người dùng thông thường
};

// Lớp đại diện cho tài khoản người dùng
class UserAccount
{
public:
    // Constructor: khởi tạo tài khoản mới với thông tin đầu vào
    UserAccount(const std::string &fullName,
                const std::string &email,
                const std::string &username,
                const std::string &password);

    // Các hàm getter – trả về thông tin người dùng
    std::string getFullName() const;
    std::string getEmail() const;
    std::string getUsername() const;
    std::string getHashedPassword() const;
    AccountStatus getStatus() const;
    UserRole getRole() const;
    bool isUsingTempPassword() const;
    std::chrono::system_clock::time_point getCreationDate() const;
    std::string getWalletAddress() const;                          // Địa chỉ ví điểm
    int getPointBalance() const;                                   // Số dư điểm
    const std::vector<std::string> &getTransactionHistory() const; // Lịch sử giao dịch

    // OTP – xác thực hai bước (mã dùng một lần)
    void setOTP(const std::string &otp, int ttlSeconds = 300); // TTL mặc định là 5 phút
    bool verifyOTP(const std::string &input) const;            // Xác thực mã OTP

    // Các hàm setter – cập nhật thông tin người dùng
    void setFullName(const std::string &fullName);
    void setEmail(const std::string &email);
    void setPassword(const std::string &password);                           // Đặt mật khẩu mới (tự động hash)
    void setHashedPassword(const std::string &hashedPassword);               // Đặt mật khẩu đã được mã hóa
    void setStatus(AccountStatus status);                                    // Cập nhật trạng thái tài khoản
    void setRole(UserRole role);                                             // Cập nhật vai trò người dùng
    void setTempPassword(bool isTemp);                                       // Đánh dấu tài khoản đang dùng mật khẩu tạm thời
    void setCreationDate(const std::chrono::system_clock::time_point &date); // Cập nhật thời gian tạo
    void setPointBalance(int points);                                        // Đặt lại số điểm
    void setWalletAddress(const std::string &address);                       // Gán địa chỉ ví thủ công (nếu cần)
    void setTransactionHistory(const std::vector<std::string> &history);     // Ghi đè toàn bộ lịch sử giao dịch

    // Thêm một giao dịch mới vào lịch sử
    void addTransaction(const std::string &log);

    // Các hàm xử lý mật khẩu
    bool validatePassword(const std::string &password) const; // Kiểm tra mật khẩu đầu vào với mật khẩu đã mã hóa
    static bool isPasswordValid(const std::string &password); // Kiểm tra độ mạnh của mật khẩu
    static std::string generateTempPassword();                // Sinh mật khẩu tạm thời ngẫu nhiên

private:
    // Thông tin cá nhân
    std::string fullName_, email_, username_, hashedPassword_;

    // Trạng thái và vai trò
    AccountStatus status_;
    UserRole role_;
    bool isTempPassword_; // Đánh dấu nếu mật khẩu hiện tại là mật khẩu tạm

    // Thời điểm tạo tài khoản
    std::chrono::system_clock::time_point creationDate_;

    // Thông tin ví
    std::string walletAddress_;                   // Địa chỉ ví duy nhất của người dùng
    int pointBalance_;                            // Số điểm hiện có
    std::vector<std::string> transactionHistory_; // Danh sách log giao dịch

    // Hàm hỗ trợ nội bộ để sinh địa chỉ ví ngẫu nhiên
    static std::string generateWalletAddress();

    // OTP hiện tại và thời điểm hết hạn
    std::string currentOTP_;
    std::chrono::system_clock::time_point otpExpiry_;
};
