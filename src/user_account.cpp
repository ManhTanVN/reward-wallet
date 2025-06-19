

#include "user_account.h"
#include <regex>
#include <stdexcept>
#include <sstream>
#include <iomanip>

UserAccount::UserAccount(const std::string &fullName,
                         const std::string &email,
                         const std::string &idNumber,
                         const std::string &username,
                         const std::string &password)
    : fullName_(fullName), email_(email), idNumber_(idNumber), username_(username), status_(AccountStatus::PENDING), isTempPassword_(false), creationDate_(std::chrono::system_clock::now())
{
    if (!password.empty())
    {
        setPassword(password);
    }
}

void UserAccount::setFullName(const std::string &fullName)
{
    if (fullName.empty())
    {
        throw std::invalid_argument("Full name cannot be empty");
    }
    fullName_ = fullName;
}

void UserAccount::setEmail(const std::string &email)
{
    // Basic email validation using regex
    const std::regex emailRegex(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    if (!std::regex_match(email, emailRegex))
    {
        throw std::invalid_argument("Invalid email format");
    }
    email_ = email;
}

void UserAccount::setPassword(const std::string &password)
{
    hashedPassword_ = DataManager::hashPassword(password);
    isTempPassword_ = false;
}

void UserAccount::setHashedPassword(const std::string &hashedPassword)
{
    hashedPassword_ = hashedPassword;
}

void UserAccount::setStatus(AccountStatus status)
{
    status_ = status;
}

void UserAccount::setTempPassword(bool isTemp)
{
    isTempPassword_ = isTemp;
}

bool UserAccount::validatePassword(const std::string &password) const
{
    return DataManager::verifyPassword(password, hashedPassword_);
}

std::string UserAccount::hashPassword(const std::string &password)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

bool UserAccount::isPasswordValid(const std::string &password)
{
    if (password.length() < 8)
        return false;

    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    for (char c : password)
    {
        if (std::isupper(c))
            hasUpper = true;
        else if (std::islower(c))
            hasLower = true;
        else if (std::isdigit(c))
            hasDigit = true;
        else
            hasSpecial = true;
    }

    return hasUpper && hasLower && hasDigit && hasSpecial;
}

std::string UserAccount::generateTempPassword()
{
    const std::string charset =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "!@#$%^&*";

    const int passwordLength = 12;
    unsigned char buf[passwordLength];
    RAND_bytes(buf, passwordLength);

    std::string tempPassword;
    tempPassword.reserve(passwordLength);

    for (int i = 0; i < passwordLength; i++)
    {
        tempPassword += charset[buf[i] % charset.length()];
    }

    // Ensure password meets requirements
    tempPassword[0] = std::toupper(charset[buf[0] % 26 + 26]); // Uppercase letter
    tempPassword[1] = std::tolower(charset[buf[1] % 26 + 26]); // Lowercase letter
    tempPassword[2] = charset[buf[2] % 10];                    // Digit
    tempPassword[3] = charset[buf[3] % 8 + 62];                // Special character

    return tempPassword;
}