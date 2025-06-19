#pragma once

#include <string>

enum class AccountStatus
{
    ACTIVE,
    SUSPENDED,
    PENDING,
    INACTIVE
};

class UserAccount
{
public:
    UserAccount(
        const std::string &fullName,
        const std::string &email,
        const std::string &idNumber,
        const std::string &username,
        const std::string &password);

    // Getters
    std::string getFullName() const { return fullName_; }
    std::string getEmail() const { return email_; }
    std::string getIdNumber() const { return idNumber_; }
    std::string getUsername() const { return username_; }
    std::string getHashedPassword() const { return hashedPassword_; }
    AccountStatus getStatus() const { return status_; }
    bool isTempPassword() const { return isTempPassword_; }
    std::chrono::system_clock::time_point getCreationDate() const { return creationDate_; }

    // Setters
    void setFullName(const std::string &fullName);
    void setEmail(const std::string &email);
    void setPassword(const std::string &password);
    void setHashedPassword(const std::string &hashedPassword);
    void setStatus(AccountStatus status);
    void setTempPassword(bool isTemp);

    // Password validation and management
    bool validatePassword(const std::string &password) const;
    static std::string hashPassword(const std::string &password);
    static bool isPasswordValid(const std::string &password);
    static std::string generateTempPassword();

private:
    std::string fullName_;
    std::string email_;
    std::string idNumber_;
    std::string username_;
    std::string hashedPassword_;
    AccountStatus status_;
    bool isTempPassword_;
    std::chrono::system_clock::time_point creationDate_;
};