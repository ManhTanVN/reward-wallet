#include "auth.h"         // Định nghĩa hàm authenticateUser
#include "otp.h"          // Quản lý mã OTP (generate, verify)
#include "user_account.h" // Định nghĩa UserAccount, gồm thông tin và xác thực
#include "data_manager.h" // Đọc/ghi user từ file
#include <iostream>       // Nhập/xuất từ bàn phím

// Hàm xác thực người dùng: kiểm tra username + password + OTP
std::shared_ptr<UserAccount> authenticateUser(
    DataManager &manager,        // Quản lý dữ liệu user từ file JSON
    const std::string &username, // Tên đăng nhập do người dùng nhập
    const std::string &password, // Mật khẩu do người dùng nhập
    bool isBackup                // Ghi lại bản backup nếu cần (cho admin)
)
{
    // Tìm user từ file dữ liệu
    auto user = manager.findUser(username);

    // Nếu không tìm thấy user hoặc mật khẩu không đúng thì trả về nullptr
    if (!user || !user->validatePassword(password))
        return nullptr;

    // Sinh mã OTP ngẫu nhiên (6 chữ số)
    std::string otp = OTPManager::generateOTP();

    // Gán OTP cho user hiện tại
    user->setOTP(otp);

    // Lưu lại thông tin user vào file (có thể tạo bản sao lưu nếu isBackup = true)
    manager.saveUser(user, isBackup);

    // In mã OTP ra màn hình (trong thực tế, nên gửi OTP qua email hoặc SMS)
    std::cout << "Your OTP: " << otp << "\n";

    // Yêu cầu người dùng nhập lại OTP
    std::string input;
    std::cout << "Enter OTP: ";
    std::getline(std::cin, input);

    // Kiểm tra OTP nhập vào có khớp với OTP đã lưu không
    if (!user->verifyOTP(input))
    {
        std::cout << "Invalid OTP.\n";
        return nullptr; // Nếu sai OTP thì trả về nullptr (thất bại)
    }

    // Nếu đúng OTP thì xác thực thành công, trả về user
    return user;
}
// Hàm này sẽ được gọi khi người dùng đăng nhập thành công
// Nó sẽ trả về một con trỏ thông minh đến UserAccount nếu xác thực thành công