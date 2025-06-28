#pragma once

#include <string>
#include <vector>
#include <chrono>

enum class AccountStatus { ACTIVE, SUSPENDED, PENDING, INACTIVE };
enum class UserRole { ADMIN, USER };

class UserAccount {
public:
    UserAccount(const std::string &fullName,
                const std::string &email,
                const std::string &username,
                const std::string &password);

                
                // Getters
                std::string getFullName() const;
                std::string getEmail() const;
                std::string getUsername() const;
                std::string getHashedPassword() const;
                AccountStatus getStatus() const;
                UserRole getRole() const;
                bool isUsingTempPassword() const;
                std::chrono::system_clock::time_point getCreationDate() const;
                
                
                
                std::string getWalletAddress() const;
                int getPointBalance() const;
                const std::vector<std::string>& getTransactionHistory() const;
    // OTP
    void setOTP(const std::string& otp, int ttlSeconds = 300);
    bool verifyOTP(const std::string& input) const;

    // Setters
    void setFullName(const std::string &fullName);
    void setEmail(const std::string &email);
    void setPassword(const std::string &password);
    void setHashedPassword(const std::string &hashedPassword);
    void setStatus(AccountStatus status);
    void setRole(UserRole role);
    void setTempPassword(bool isTemp);
    void setCreationDate(const std::chrono::system_clock::time_point &date);
    void setPointBalance(int points);
    void setWalletAddress(const std::string& address);
    void setTransactionHistory(const std::vector<std::string>& history);

    // Wallet operations
    void addTransaction(const std::string& log);

    // Password utility
    bool validatePassword(const std::string &password) const;
    static bool isPasswordValid(const std::string &password);
    static std::string generateTempPassword();

private:
    std::string fullName_, email_, username_, hashedPassword_;
    AccountStatus status_;
    UserRole role_;
    bool isTempPassword_;
    std::chrono::system_clock::time_point creationDate_;

    std::string walletAddress_;
    int pointBalance_;
    std::vector<std::string> transactionHistory_;

    static std::string generateWalletAddress();  // Private helper

    // OTP
    std::string currentOTP_;
    std::chrono::system_clock::time_point otpExpiry_;
};
