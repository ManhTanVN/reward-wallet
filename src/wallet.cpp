#include "wallet.h"
#include "otp.h"
#include "utils.h"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool transferPoints(DataManager& manager,
                    const std::string& fromWallet,
                    const std::string& toWallet,
                    int amount,
                    const std::string& note)
{
    if (fromWallet == toWallet || amount <= 0) return false;

    // Tìm người gửi và người nhận
    auto from = manager.findUserByWallet(fromWallet);
    auto to = manager.findUserByWallet(toWallet);

    if (!from || !to) {
        std::cout << "[ERROR] Sender or receiver not found.\n";
        return false;
    }

    if (from->getPointBalance() < amount) {
        std::cout << "[ERROR] Not enough balance.\n";
        return false;
    }

    // ✅ OTP xác thực nếu không phải ví master
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

    // Trừ và cộng điểm
    from->setPointBalance(from->getPointBalance() - amount);
    to->setPointBalance(to->getPointBalance() + amount);

    std::string timestamp = currentTimestamp();
    std::string sendTxType = "transfer";
    std::string receiveTxType = (note == "Buy points") ? "buy" : "transfer";

    json sendLog = {
        {"amount", amount},
        {"incoming", false},
        {"otherWallet", to->getWalletAddress()},
        {"timestamp", timestamp},
        {"note", note},
        {"type", sendTxType}
    };

    json receiveLog = {
        {"amount", amount},
        {"incoming", true},
        {"otherWallet", from->getWalletAddress()},
        {"timestamp", timestamp},
        {"note", note},
        {"type", receiveTxType}
    };

    from->addTransaction(sendLog.dump());
    to->addTransaction(receiveLog.dump());

    // Lưu cả 2 người dùng
    manager.saveUser(from);
    manager.saveUser(to);

    std::cout << "Transferred " << amount << " points from " << from->getUsername()
              << " to " << to->getUsername() << ".\n";
    return true;
}
