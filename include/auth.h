#pragma once

#include "user_account.h"
#include "data_manager.h"
#include <string>
#include <memory>

// Đăng nhập
std::shared_ptr<UserAccount> authenticateUser(DataManager& manager, const std::string& username, const std::string& password);

// Kiểm tra có đang dùng mật khẩu tạm không
bool requirePasswordChange(std::shared_ptr<UserAccount> user);

// Đổi mật khẩu (nếu cần cập nhật mật khẩu ngay trong CLI)
// void changePassword(std::shared_ptr<UserAccount> user, DataManager& manager);
