#pragma once

#include "user_account.h" // Định nghĩa lớp UserAccount: quản lý thông tin người dùng, mật khẩu, OTP,...
#include "data_manager.h" // Lớp quản lý đọc/ghi dữ liệu người dùng từ file
#include <string>         // Sử dụng std::string
#include <memory>         // Sử dụng std::shared_ptr

// Hàm xác thực người dùng bao gồm kiểm tra mật khẩu và mã OTP.
// Trả về con trỏ UserAccount nếu xác thực thành công, ngược lại trả về  nullptr.
// Tham số isBackup cho biết có ghi bản sao lưu sau khi sinh OTP hay không (mặc định có).
std::shared_ptr<UserAccount> authenticateUser(
    DataManager &manager,        // Quản lý và truy xuất người dùng từ file
    const std::string &username, // Tên đăng nhập do người dùng cung cấp
    const std::string &password, // Mật khẩu do người dùng nhập
    bool isBackup = true         // Có tạo bản sao lưu sau khi set OTP hay không
);

// Hàm kiểm tra xem người dùng có đang sử dụng mật khẩu tạm thời hay không.
// Trả về true nếu cần yêu cầu người dùng đổi mật khẩu sau khi đăng nhập.
bool requirePasswordChange(std::shared_ptr<UserAccount> user);
// Hàm này sẽ được gọi khi người dùng đăng nhập thành công
// Nó sẽ trả về một con trỏ thông minh đến UserAccount nếu xác thực thành công