#include "user_account.h"
#include "data_manager.h"
#include "sha256.h"
#include <regex>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cstdlib>  // rand(), srand()
#include <ctime>    // time()
#include <cctype>

UserAccount::UserAccount(const std::string &fullName,
                         const std::string &email,
                         const std::string &idNumber,
                         const std::string &username,
                         const std::string &password)
    : fullName_(fullName), email_(email), idNumber_(idNumber), username_(username),
      status_(AccountStatus::PENDING), isTempPassword_(false), creationDate_(std::chrono::system_clock::now())
{
    if (!password.empty()) {
        setPassword(password);
    }
}

// Getters
std::string UserAccount::getFullName() const { return fullName_; }
std::string UserAccount::getEmail() const { return email_; }
std::string UserAccount::getIdNumber() const { return idNumber_; }
std::string UserAccount::getUsername() const { return username_; }
std::string UserAccount::getHashedPassword() const { return hashedPassword_; }
AccountStatus UserAccount::getStatus() const { return status_; }
bool UserAccount::isUsingTempPassword() const { return isTempPassword_; }
std::chrono::system_clock::time_point UserAccount::getCreationDate() const { return creationDate_; }

// Setters
void UserAccount::setFullName(const std::string &fullName) {
    if (fullName.empty()) {
        throw std::invalid_argument("Full name cannot be empty");
    }
    fullName_ = fullName;
}

void UserAccount::setEmail(const std::string &email) {
    const std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!std::regex_match(email, emailRegex)) {
        throw std::invalid_argument("Invalid email format");
    }
    email_ = email;
}

void UserAccount::setPassword(const std::string &password) {
    hashedPassword_ = DataManager::hashPassword(password);
    isTempPassword_ = false;
}

void UserAccount::setHashedPassword(const std::string &hashedPassword) {
    hashedPassword_ = hashedPassword;
}

void UserAccount::setStatus(AccountStatus status) {
    status_ = status;
}

void UserAccount::setTempPassword(bool isTemp) {
    isTempPassword_ = isTemp;
}

void UserAccount::setCreationDate(const std::chrono::system_clock::time_point &date) {
    creationDate_ = date;
}

// Password validation
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
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "!@#$%^&*";

    const int passwordLength = 12;
    std::string tempPassword;
    tempPassword.reserve(passwordLength);

    std::srand(static_cast<unsigned int>(std::time(nullptr)));  // Seed for rand()

    for (int i = 0; i < passwordLength; ++i) {
        tempPassword += charset[std::rand() % charset.length()];
    }

    // Ensure at least one character of each type
    tempPassword[0] = 'A'; // Uppercase
    tempPassword[1] = 'a'; // Lowercase
    tempPassword[2] = '9'; // Digit
    tempPassword[3] = '!'; // Special character

    return tempPassword;
}
