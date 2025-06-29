#include "wallet.h"
#include "otp.h"
#include "utils.h"
#include <iostream>
#include <nlohmann/json.hpp> // Dùng thư viện JSON để ghi log giao dịch

using json = nlohmann::json; // Đặt alias cho nlohmann::json để dùng ngắn gọn

// Hàm chuyển điểm từ ví người dùng này sang người dùng khác
bool transferPoints(DataManager &manager,
                    const std::string &fromWallet,
                    const std::string &toWallet,
                    int amount,
                    const std::string &note)
{
    // Kiểm tra điều kiện cơ bản: không tự chuyển cho chính mình và số lượng phải > 0
    if (fromWallet == toWallet || amount <= 0)
        return false;

    // Tìm người gửi và người nhận thông qua địa chỉ ví
    auto from = manager.findUserByWallet(fromWallet);
    auto to = manager.findUserByWallet(toWallet);

    // Nếu không tìm thấy người gửi hoặc người nhận thì báo lỗi
    if (!from || !to)
    {
        std::cout << "[ERROR] Sender or receiver not found.\n";
        return false;
    }

    // Kiểm tra số dư có đủ để chuyển hay không
    if (from->getPointBalance() < amount)
    {
        std::cout << "[ERROR] Not enough balance.\n";
        return false;
    }

    // Nếu người gửi không phải là ví master thì yêu cầu xác thực OTP
    if (from->getUsername() != "__master__wallet__")
    {
        std::string otp = OTPManager::generateOTP(); // Sinh OTP
        from->setOTP(otp);                           // Lưu OTP vào user
        manager.saveUser(from);                      // Ghi lại thông tin đã cập nhật

        std::cout << "[OTP] Verification required. Your OTP: " << otp << "\n";

        std::string input;
        std::cout << "Enter OTP to confirm transfer: ";
        std::getline(std::cin, input); // Đọc OTP từ người dùng

        if (!from->verifyOTP(input))
        {
            std::cout << "[ERROR] Invalid OTP. Transfer aborted.\n";
            return false;
        }
    }

    // Cập nhật số dư điểm: trừ người gửi, cộng người nhận
    from->setPointBalance(from->getPointBalance() - amount);
    to->setPointBalance(to->getPointBalance() + amount);

    std::string timestamp = currentTimestamp(); // Ghi thời gian giao dịch

    // Gắn loại giao dịch cho từng bên
    std::string sendTxType = "transfer";                                     // Người gửi luôn là "transfer"
    std::string receiveTxType = (note == "Buy points") ? "buy" : "transfer"; // Nếu ghi chú là "Buy points" thì bên nhận là mua

    // Tạo log JSON cho người gửi
    json sendLog = {
        {"amount", amount},
        {"incoming", false}, // outgoing
        {"otherWallet", to->getWalletAddress()},
        {"timestamp", timestamp},
        {"note", note},
        {"type", sendTxType}};

    // Tạo log JSON cho người nhận
    json receiveLog = {
        {"amount", amount},
        {"incoming", true}, // incoming
        {"otherWallet", from->getWalletAddress()},
        {"timestamp", timestamp},
        {"note", note},
        {"type", receiveTxType}};

    // Lưu log giao dịch vào lịch sử của từng người
    from->addTransaction(sendLog.dump());
    to->addTransaction(receiveLog.dump());

    // Lưu thông tin người dùng đã cập nhật vào file JSON
    manager.saveUser(from);
    manager.saveUser(to);

    // In thông báo thành công
    std::cout << "Transferred " << amount << " points from " << from->getUsername()
              << " to " << to->getUsername() << ".\n";

    return true;
}
