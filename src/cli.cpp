#include <nlohmann/json.hpp>
#include "cli.h"
#include "auth.h"
#include "wallet.h"
#include "otp.h"
#include "utils.h"
#include <iostream>
#include <limits>

using json = nlohmann::json;

int countUsersReceivedFromMaster(DataManager &manager)
{
    auto users = manager.loadAllUsers();
    int count = 0;
    for (const auto &user : users)
    {
        for (const auto &log : user->getTransactionHistory())
        {
            if (log.find("\"from\":\"__master__wallet__\"") != std::string::npos)
            {
                ++count;
                break;
            }
        }
    }
    return count;
}

void showMainMenu()
{
    std::cout << "\n========= Reward Wallet =========\n";
    std::cout << "1. Register new user\n";
    std::cout << "2. Login\n";
    std::cout << "0. Exit\n";
    std::cout << "===============================\n";
    std::cout << "Enter your choice: ";
}

void handleUserInput(DataManager &manager)
{
    int choice;
    do
    {
        showMainMenu();
        // std::cin >> choice;
        // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        choice = getValidatedInput("Enter your choice: ", 0, 2);
        switch (choice)
        {
        case 1:
            registerUser(manager);
            break;
        case 2:
        {
            auto user = loginUser(manager);
            if (user)
            {
                showUserMenu(user, manager);
            }
            break;
        }
        case 0:
            std::cout << "Goodbye!\n";
            break;
        default:
            std::cout << "Invalid choice.\n";
        }

    } while (choice != 0);
}

void registerUser(DataManager &manager)
{
    std::string name, email, username, password;

    std::cout << "Full Name: ";
    std::getline(std::cin, name);
    std::cout << "Email: ";
    std::getline(std::cin, email);
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << "Password: ";
    std::getline(std::cin, password);

    if (!UserAccount::isPasswordValid(password))
    {
        std::cout << "Weak password.\n";
        return;
    }

    auto user = std::make_shared<UserAccount>(name, email, username, password);
    user->setRole(UserRole::USER);
    manager.saveUser(user);

    auto master = manager.findUser("__master__wallet__");
    if (master && countUsersReceivedFromMaster(manager) < 10 && master->getPointBalance() >= 1000)
    {
        if (transferPoints(manager, master->getWalletAddress(), user->getWalletAddress(), 1000, "reward"))
        {
            std::cout << "User received 1000 points from master wallet\n";
        }
    }

    std::cout << "User registered successfully.\n";
}
std::shared_ptr<UserAccount> loginUser(DataManager &manager)
{
    std::string username, password;
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << "Password: ";
    std::getline(std::cin, password);

    auto user = authenticateUser(manager, username, password, false);
    if (!user)
    {
        std::cout << "Login failed.\n";
        return nullptr;
    }

    std::cout << "Login successful. Welcome, " << user->getUsername() << "!\n";

    if (user->isUsingTempPassword())
    {
        std::cout << "You are using a temporary password. Please change it now.\n";
        changePassword(user, manager);
    }

    return user;
}

// Nap points
void buyPoints(std::shared_ptr<UserAccount> buyer, DataManager &manager)
{
    int amount;
    std::cout << "Enter amount of points to buy: ";
    std::cin >> amount;
    std::cin.ignore();

    if (amount <= 0)
    {
        std::cout << "Invalid amount.\n";
        return;
    }

    // Xác minh  lại mật khẩu
    std::string password;
    std::cout << "Re-enter your password: ";
    std::getline(std::cin, password);
    if (!buyer->validatePassword(password))
    {
        std::cout << "Password incorrect. Purchase aborted.\n";
        return;
    }

    // Xác thực OTP
    std::string otp = OTPManager::generateOTP();
    buyer->setOTP(otp);
    manager.saveUser(buyer); // Lưu OTP vào hệ thống
    std::cout << "Your OTP: " << otp << "\n";

    std::string input;
    std::cout << "Enter OTP: ";
    std::getline(std::cin, input);
    if (!buyer->verifyOTP(input))
    {
        std::cout << "Invalid OTP. Purchase aborted.\n";
        return;
    }

    // Tìm ví master
    auto master = manager.findUser("__master__wallet__");
    if (!master)
    {
        std::cout << "[ERROR] Master wallet not found.\n";
        return;
    }

    // 1. Tăng điểm (mint) cho ví master
    master->setPointBalance(master->getPointBalance() + amount);

    // Ghi log mint
    json mintLog = {
        {"amount", amount},
        {"incoming", true},
        {"otherWallet", "SYSTEM"},
        {"timestamp", currentTimestamp()},
        {"note", "Minted for sale"},
        {"type", "mint"}};
    master->addTransaction(mintLog.dump());
    manager.saveUser(master); // Lưu lại điểm và log mint

    // 2. Chuyển điểm từ master sang buyer
    bool success = transferPoints(manager,
                                  master->getWalletAddress(),
                                  buyer->getWalletAddress(),
                                  amount,
                                  "Buy points");

    if (success)
    {
        std::cout << "Purchased " << amount << " points successfully.\n";

        // Reload lại buyer để cập nhật số dư mới nhất
        buyer = manager.findUser(buyer->getUsername());
    }
    else
    {
        std::cout << "Transfer failed after minting. Check balances.\n";
    }
}

void showUserMenu(const std::shared_ptr<UserAccount> &user, DataManager &manager)
{
    int choice;
    do
    {
        std::cout << "\n--- " << (user->getRole() == UserRole::ADMIN ? "[Admin]" : "[User]") << " Menu ---\n";

        if (user->getRole() == UserRole::ADMIN)
        {
            std::cout << "1. List all users\n";
            std::cout << "2. Delete user\n";
            std::cout << "3. Create multiple users (bulk)\n";
        }

        std::cout << "4. Change password\n";
        std::cout << "5. Show balance\n";
        std::cout << "6. Send points\n";
        std::cout << "7. View transaction history\n";
        std::cout << "8. Buy points\n";
        if (user->getRole() == UserRole::ADMIN)
        {
            std::cout << "9. Backup data\n";
            std::cout << "10. Restore from backup\n";
        }
        std::cout << "0. Logout\n";
        std::cout << "============================\n";
        std::cout << "Choice: ";

        // std::cin >> choice;
        // std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        choice = getValidatedInput("Enter your choice: ", 0, 10);

        switch (choice)
        {
        case 1:
            if (user->getRole() == UserRole::ADMIN)
                listUsers(manager);
            else
                std::cout << "Permission denied.\n";
            break;
        case 2:
            if (user->getRole() == UserRole::ADMIN)
                deleteUser(manager);
            else
                std::cout << "Permission denied.\n";
            break;
        case 3:
            if (user->getRole() == UserRole::ADMIN)
                createMultipleUsers(manager);
            else
                std::cout << "Permission denied.\n";
            break;
        case 4:
            changePassword(user, manager);
            break;
        case 5:
            showBalance(user, manager);
            break;
        case 6:
            transferPointsCLI(user, manager);
            break;
        case 7:
            viewTransactionHistory(user, manager);
            break;
        case 8:
            buyPoints(user, manager);
            break;
        case 9:
            if (user->getRole() == UserRole::ADMIN)
                createBackup("data/users.json", "backups");
            else
                std::cout << "Permission denied.\n";
            break;
        case 10:
            if (user->getRole() == UserRole::ADMIN)
                listBackupFiles("backups");
            else
                std::cout << "Permission denied.\n";
            break;
        case 0:
            std::cout << "Logging out...\n";
            break;
        default:
            std::cout << "Invalid.\n";
        }

    } while (choice != 0);
}

void listUsers(DataManager &manager)
{
    auto users = manager.loadAllUsers();
    for (const auto &u : users)
    {
        std::cout << "- " << u->getUsername() << " ["
                  << (u->getRole() == UserRole::ADMIN ? "admin" : "user") << "] Wallet: "
                  << u->getWalletAddress() << " - " << u->getPointBalance() << " pts\n";
    }
}

void deleteUser(DataManager &manager)
{
    std::string username;
    std::cout << "Username to delete: ";
    std::getline(std::cin, username);
    manager.removeUser(username);
    std::cout << "Deleted (if existed).\n";
}

void createMultipleUsers(DataManager &manager)
{
    int n;
    std::cout << "How many users to create? ";
    std::cin >> n;
    std::cin.ignore();

    auto master = manager.findUser("__master__wallet__");
    if (!master)
    {
        std::cout << "[ERROR] Master wallet not found!\n";
        return;
    }

    int alreadyRewarded = countUsersReceivedFromMaster(manager);

    for (int i = 0; i < n; ++i)
    {
        std::string name, email, username;
        std::cout << "Full Name: ";
        std::getline(std::cin, name);
        std::cout << "Email: ";
        std::getline(std::cin, email);
        std::cout << "Username: ";
        std::getline(std::cin, username);

        std::string tempPwd = UserAccount::generateTempPassword();
        auto user = std::make_shared<UserAccount>(name, email, username, tempPwd);
        user->setRole(UserRole::USER);
        user->setTempPassword(true);
        manager.saveUser(user);

        if (alreadyRewarded < 10 && master->getPointBalance() >= 1000)
        {
            if (transferPoints(manager, master->getWalletAddress(), user->getWalletAddress(), 1000, "reward"))
            {
                ++alreadyRewarded;
                std::cout << "[+] Gave 1000 pts to " << username << " from master wallet.\n";
            }
        }

        std::cout << "Created " << username << " | Temp password: " << tempPwd << "\n";
    }
}

void changePassword(std::shared_ptr<UserAccount> user, DataManager &manager)
{
    std::string otp = OTPManager::generateOTP();
    user->setOTP(otp);
    manager.saveUser(user);
    std::cout << "Your OTP: " << otp << "\n";

    std::string input;
    std::cout << "Enter OTP: ";
    std::getline(std::cin, input);
    if (!user->verifyOTP(input))
    {
        std::cout << "Invalid OTP.\n";
        return;
    }

    std::string newPwd;
    std::cout << "New password: ";
    std::getline(std::cin, newPwd);

    if (!UserAccount::isPasswordValid(newPwd))
    {
        std::cout << "Weak password.\n";
        return;
    }

    user->setPassword(newPwd);
    user->setTempPassword(false);
    manager.saveUser(user);
    std::cout << "Password changed.\n";
}

void showBalance(const std::shared_ptr<UserAccount> &user, DataManager &manager)
{
    auto refreshedUser = manager.findUser(user->getUsername());
    if (!refreshedUser)
    {
        std::cout << "[ERROR] Cannot find user.\n";
        return;
    }

    std::cout << "Wallet address: " << refreshedUser->getWalletAddress() << "\n";
    std::cout << "Balance: " << refreshedUser->getPointBalance() << " points\n";
}

void transferPointsCLI(std::shared_ptr<UserAccount> sender, DataManager &manager)
{
    if (sender->getUsername() != "__master__wallet__")
    {
        std::string otp = OTPManager::generateOTP();
        sender->setOTP(otp);
        manager.saveUser(sender);
        std::cout << "OTP: " << otp << "\n";
        std::string input;
        std::cout << "Enter OTP: ";
        std::getline(std::cin, input);
        if (!sender->verifyOTP(input))
        {
            std::cout << "Invalid OTP.\n";
            return;
        }
    }

    std::string receiverAddress;
    int amount;
    std::cout << "Receiver Wallet Address: ";
    std::getline(std::cin, receiverAddress);
    std::cout << "Amount to send: ";
    std::cin >> amount;
    std::cin.ignore();

    if (transferPoints(manager, sender->getWalletAddress(), receiverAddress, amount))
    {
        std::cout << "Transfer successful.\n";
    }
    else
    {
        std::cout << "Transfer failed. Check balance or address.\n";
    }
}

void viewTransactionHistory(const std::shared_ptr<UserAccount> &user, DataManager &manager)
{
    auto refreshedUser = manager.findUser(user->getUsername());
    if (!refreshedUser)
    {
        std::cout << "[ERROR] Cannot load user from file.\n";
        return;
    }

    const auto &logs = refreshedUser->getTransactionHistory();
    if (logs.empty())
    {
        std::cout << "No transactions.\n";
        return;
    }

    for (const auto &logStr : logs)
    {
        try
        {
            json log = json::parse(logStr);
            std::string type = log.value("type", "transfer");
            bool incoming = log.value("incoming", false);
            std::string otherWallet = log.value("otherWallet", "N/A");
            int amount = log.value("amount", 0);
            std::string note = log.value("note", "");
            std::string timestamp = log.value("timestamp", "");

            if (type == "buy" && incoming)
            {
                std::cout << "[BUY] +" << amount << " points from Master";
            }
            else if (incoming)
            {
                std::cout << "[RECEIVED] from " << otherWallet << " - +" << amount << " points";
            }
            else
            {
                std::cout << "[SENT] to " << otherWallet << " - -" << amount << " points";
            }

            if (!note.empty())
                std::cout << " | Note: " << note;
            if (!timestamp.empty())
                std::cout << " | Time: " << timestamp;
            std::cout << "\n";
        }
        catch (const std::exception &e)
        {
            std::cout << "[Invalid log format] " << e.what() << "\n";
        }
    }
}
