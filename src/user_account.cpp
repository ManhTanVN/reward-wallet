#include "user_account.h"
#include "data_manager.h"
#include "sha256.h"
#include "otp.h"
#include <regex>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>

// Hàm khởi tạo của lớp UserAccount
UserAccount::UserAccount(const std::string &fullName,
                         const std::string &email,
                         const std::string &username,
                         const std::string &password)
    : fullName_(fullName), email_(email),
      username_(username), status_(AccountStatus::PENDING), // Trạng thái mặc định là PENDING
      role_(UserRole::USER),                                // Quyền mặc định là USER
      isTempPassword_(false),                               // Mặc định không dùng mật khẩu tạm
      creationDate_(std::chrono::system_clock::now()),      // Ghi nhận thời gian tạo tài khoản
      walletAddress_(generateWalletAddress()),              // Sinh địa chỉ ví ngẫu nhiên
      pointBalance_(0)                                      // Số điểm ban đầu là 0
{
    if (!password.empty())
        setPassword(password); // Nếu có mật khẩu truyền vào thì mã hóa và lưu
}

// ==================== Getter ====================
// Các hàm lấy thông tin của tài khoản
std::string UserAccount::getFullName() const { return fullName_; }
std::string UserAccount::getEmail() const { return email_; }
std::string UserAccount::getUsername() const { return username_; }
std::string UserAccount::getHashedPassword() const { return hashedPassword_; }
AccountStatus UserAccount::getStatus() const { return status_; }
UserRole UserAccount::getRole() const { return role_; }
bool UserAccount::isUsingTempPassword() const { return isTempPassword_; }
std::chrono::system_clock::time_point UserAccount::getCreationDate() const { return creationDate_; }
std::string UserAccount::getWalletAddress() const { return walletAddress_; }
int UserAccount::getPointBalance() const { return pointBalance_; }
const std::vector<std::string> &UserAccount::getTransactionHistory() const { return transactionHistory_; }

// ==================== OTP ====================
// Cài đặt OTP cho user với thời hạn (ttlSeconds)
void UserAccount::setOTP(const std::string &otp, int ttlSeconds)
{
    currentOTP_ = otp;
    otpExpiry_ = std::chrono::system_clock::now() + std::chrono::seconds(ttlSeconds); // Thiết lập thời điểm hết hạn
}

// Xác thực OTP được nhập
bool UserAccount::verifyOTP(const std::string &input) const
{
    return OTPManager::isOTPValid(currentOTP_, input, otpExpiry_); // OTPManager xử lý so sánh & kiểm tra hạn
}

// ==================== Setter ====================
// Các hàm cập nhật thông tin user
void UserAccount::setFullName(const std::string &fullName) { fullName_ = fullName; }
void UserAccount::setEmail(const std::string &email) { email_ = email; }

void UserAccount::setPassword(const std::string &password)
{
    hashedPassword_ = DataManager::hashPassword(password); // Mã hóa mật khẩu trước khi lưu
    isTempPassword_ = false;                               // Sau khi set, mật khẩu này là thật
}

void UserAccount::setHashedPassword(const std::string &hashedPassword) { hashedPassword_ = hashedPassword; }
void UserAccount::setStatus(AccountStatus status) { status_ = status; }
void UserAccount::setRole(UserRole role) { role_ = role; }
void UserAccount::setTempPassword(bool isTemp) { isTempPassword_ = isTemp; }
void UserAccount::setCreationDate(const std::chrono::system_clock::time_point &date) { creationDate_ = date; }
void UserAccount::setPointBalance(int points) { pointBalance_ = points; }
void UserAccount::setWalletAddress(const std::string &address) { walletAddress_ = address; }
void UserAccount::setTransactionHistory(const std::vector<std::string> &history) { transactionHistory_ = history; }

// Thêm log giao dịch vào lịch sử
void UserAccount::addTransaction(const std::string &log) { transactionHistory_.push_back(log); }

// ==================== Wallet ====================
// Hàm sinh địa chỉ ví ngẫu nhiên (12 ký tự hex) dạng "0xABC123..."
std::string UserAccount::generateWalletAddress()
{
    static const char hexChars[] = "0123456789ABCDEF"; // Các ký tự hex
    std::stringstream ss;
    ss << "0x"; // Prefix bắt buộc của địa chỉ ví
    std::random_device rd;
    std::mt19937 gen(rd());                      // Bộ sinh số ngẫu nhiên Mersenne Twister
    std::uniform_int_distribution<> dist(0, 15); // Sinh số trong khoảng 0-15 (hex)
    for (int i = 0; i < 12; ++i)
        ss << hexChars[dist(gen)]; // Ghép ký tự hex ngẫu nhiên
    return ss.str();
}

// ==================== Password ====================
// Sinh mật khẩu tạm thời ngẫu nhiên (12 ký tự)
std::string UserAccount::generateTempPassword()
{
    const std::string charset =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*"; // Bộ ký tự cho random
    const int len = 12;
    std::string temp;
    std::srand(static_cast<unsigned>(std::time(nullptr))); // Seed ngẫu nhiên
    for (int i = 0; i < len; ++i)
        temp += charset[std::rand() % charset.length()]; // Random ký tự bất kỳ

    // Ép buộc đảm bảo mật khẩu có đủ loại ký tự (hoa, thường, số, đặc biệt)
    temp[0] = 'A';
    temp[1] = 'a';
    temp[2] = '9';
    temp[3] = '#';
    return temp;
}

// Kiểm tra độ mạnh mật khẩu
bool UserAccount::isPasswordValid(const std::string &password)
{
    if (password.length() < 8)
        return false; // Độ dài tối thiểu 8 ký tự

    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char c : password)
    {
        if (std::isupper(c))
            hasUpper = true;
        else if (std::islower(c))
            hasLower = true;
        else if (std::isdigit(c))
            hasDigit = true;
        else
            hasSpecial = true; // Nếu không thuộc các loại trên thì coi là ký tự đặc biệt
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

// Xác thực mật khẩu (so sánh với hash đã lưu)
bool UserAccount::validatePassword(const std::string &password) const
{
    return DataManager::verifyPassword(password, hashedPassword_);
}


