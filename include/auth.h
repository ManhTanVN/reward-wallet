#pragma once

#include "user_account.h"
#include "data_manager.h"
#include <string>
#include <memory>

// Đăng nhập (bao gồm xác thực mật khẩu và OTP)
std::shared_ptr<UserAccount> authenticateUser(DataManager& manager, const std::string& username, const std::string& password, bool isBackup = true);

// Kiểm tra xem user có đang dùng mật khẩu tạm không
bool requirePasswordChange(std::shared_ptr<UserAccount> user);
