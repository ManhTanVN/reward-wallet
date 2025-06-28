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

// Constructor
UserAccount::UserAccount(const std::string &fullName,
                         const std::string &email,
                         const std::string &username,
                         const std::string &password)
    : fullName_(fullName), email_(email),
      username_(username), status_(AccountStatus::PENDING),
      role_(UserRole::USER), isTempPassword_(false),
      creationDate_(std::chrono::system_clock::now()),
      walletAddress_(generateWalletAddress()), pointBalance_(0)
{
    if (!password.empty()) setPassword(password);
}

// Getters
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
const std::vector<std::string>& UserAccount::getTransactionHistory() const { return transactionHistory_; }

// OTP
void UserAccount::setOTP(const std::string& otp, int ttlSeconds) {
    currentOTP_ = otp;
    otpExpiry_ = std::chrono::system_clock::now() + std::chrono::seconds(ttlSeconds);
}

bool UserAccount::verifyOTP(const std::string& input) const {
    return OTPManager::isOTPValid(currentOTP_, input, otpExpiry_);
}


// Setters
void UserAccount::setFullName(const std::string &fullName) { fullName_ = fullName; }
void UserAccount::setEmail(const std::string &email) { email_ = email; }
void UserAccount::setPassword(const std::string &password) {
    hashedPassword_ = DataManager::hashPassword(password);
    isTempPassword_ = false;
}
void UserAccount::setHashedPassword(const std::string &hashedPassword) { hashedPassword_ = hashedPassword; }
void UserAccount::setStatus(AccountStatus status) { status_ = status; }
void UserAccount::setRole(UserRole role) { role_ = role; }
void UserAccount::setTempPassword(bool isTemp) { isTempPassword_ = isTemp; }
void UserAccount::setCreationDate(const std::chrono::system_clock::time_point &date) { creationDate_ = date; }
void UserAccount::setPointBalance(int points) { pointBalance_ = points; }
void UserAccount::setWalletAddress(const std::string& address) { walletAddress_ = address; }
void UserAccount::setTransactionHistory(const std::vector<std::string>& history) { transactionHistory_ = history; }
void UserAccount::addTransaction(const std::string& log) { transactionHistory_.push_back(log); }

// Wallet address generator
std::string UserAccount::generateWalletAddress() {
    static const char hexChars[] = "0123456789ABCDEF";
    std::stringstream ss;
    ss << "0x";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 15);
    for (int i = 0; i < 12; ++i)
        ss << hexChars[dist(gen)];
    return ss.str();
}

// Password logic
bool UserAccount::validatePassword(const std::string &password) const {
    return DataManager::verifyPassword(password, hashedPassword_);
}

bool UserAccount::isPasswordValid(const std::string &password) {
    if (password.length() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char c : password) {
        if (std::isupper(c)) hasUpper = true;
        else if (std::islower(c)) hasLower = true;
        else if (std::isdigit(c)) hasDigit = true;
        else hasSpecial = true;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

std::string UserAccount::generateTempPassword() {
    const std::string charset =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!@#$%^&*";
    const int len = 12;
    std::string temp;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    for (int i = 0; i < len; ++i)
        temp += charset[std::rand() % charset.length()];
    temp[0] = 'A'; temp[1] = 'a'; temp[2] = '9'; temp[3] = '#';
    return temp;
}
