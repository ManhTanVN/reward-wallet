#pragma once

#include <string>
#include <chrono>

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
    std::string getFullName() const;
    std::string getEmail() const;
    std::string getIdNumber() const;
    std::string getUsername() const;
    std::string getHashedPassword() const;
    AccountStatus getStatus() const;
    bool isUsingTempPassword() const;
    std::chrono::system_clock::time_point getCreationDate() const;

    // Setters
    void setFullName(const std::string &fullName);
    void setEmail(const std::string &email);
    void setPassword(const std::string &password);                 // Gọi hash bên DataManager
    void setHashedPassword(const std::string &hashedPassword);     // Dành cho việc load từ file
    void setStatus(AccountStatus status);
    void setTempPassword(bool isTemp);
    void setCreationDate(const std::chrono::system_clock::time_point &date);

    // Password validation and generation
    bool validatePassword(const std::string &password) const;
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
