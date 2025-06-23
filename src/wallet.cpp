#include "wallet.h"
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

    from->addTransaction(sendLog.dump());     // lưu chuỗi JSON
    to->addTransaction(receiveLog.dump());

    manager.saveUser(from);
    manager.saveUser(to);
    return true;
}
