#include "wallet.h"
#include "otp.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string currentTime() {
    std::time_t now = std::time(nullptr);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now), "%F %T");
    return ss.str();
}

bool transferPoints(DataManager& manager,
                    const std::string& fromWallet,
                    const std::string& toWallet,
                    int amount)
{
    if (fromWallet == toWallet || amount <= 0) return false;

    auto users = manager.loadAllUsers();
    std::shared_ptr<UserAccount> from = nullptr, to = nullptr;

    for (auto& u : users) {
        if (u->getWalletAddress() == fromWallet) from = u;
        if (u->getWalletAddress() == toWallet) to = u;
    }

    if (!from || !to) return false;
    if (from->getPointBalance() < amount) return false;

    // ✅ Bỏ qua OTP nếu là ví master
    if (from->getUsername() != "__master__wallet__") {
        std::string otp = OTPManager::generateOTP();
        from->setOTP(otp);
        manager.saveUser(from);
        std::cout << "[OTP] Verification required. Your OTP: " << otp << "\n";

        std::string input;
        std::cout << "Enter OTP to confirm transfer: ";
        std::getline(std::cin, input);
        if (!from->verifyOTP(input)) {
            std::cout << "[ERROR] Invalid OTP. Transfer aborted.\n";
            return false;
        }
    }

    from->setPointBalance(from->getPointBalance() - amount);
    to->setPointBalance(to->getPointBalance() + amount);

    std::string time = currentTime();

    // Ghi log dưới dạng JSON
    json sendLog = {
        {"incoming", false},
        {"otherWallet", toWallet},
        {"amount", amount},
        {"note", "Sent at " + time}
    };
    json receiveLog = {
        {"incoming", true},
        {"otherWallet", fromWallet},
        {"amount", amount},
        {"note", "Received at " + time}
    };

    from->addTransaction(sendLog.dump());
    to->addTransaction(receiveLog.dump());

    manager.saveUser(from);
    manager.saveUser(to);
    return true;
}
