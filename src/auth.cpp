#include "auth.h"
#include "otp.h"
#include "user_account.h"
#include "data_manager.h"
#include <iostream>

// std::shared_ptr<UserAccount> authenticateUser(DataManager& manager, const std::string& username, const std::string& password)
// {
//     auto user = manager.findUser(username);
//     if (!user || !user->validatePassword(password)) {
//         return nullptr;
//     }

//     // Nếu đúng mật khẩu, tiếp tục xác thực bằng OTP
//     std::string otp = OTPManager::generateOTP();
//     user->setOTP(otp);
//     manager.saveUser(user);  // lưu lại OTP mới

//     std::cout << "Your OTP: " << otp << "\n";  // THỰC TẾ: gửi email
//     std::string input;
//     std::cout << "Enter OTP: ";
//     std::getline(std::cin, input);

//     if (!user->verifyOTP(input)) {
//         std::cout << "Invalid OTP.\n";
//         return nullptr;
//     }

//     return user;
// }
std::shared_ptr<UserAccount> authenticateUser(DataManager& manager, const std::string& username, const std::string& password, bool isBackup) {
    auto user = manager.findUser(username);
    if (!user || !user->validatePassword(password))
        return nullptr;

    std::string otp = OTPManager::generateOTP();
    user->setOTP(otp);
    manager.saveUser(user, isBackup);

    std::cout << "Your OTP: " << otp << "\n"; // sản phẩm thật thì gửi email

    std::string input;
    std::cout << "Enter OTP: ";
    std::getline(std::cin, input);

    if (!user->verifyOTP(input)) {
        std::cout << "Invalid OTP.\n";
        return nullptr;
    }

    return user;
}

