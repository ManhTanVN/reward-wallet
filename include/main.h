#pragma once

#include "data_manager.h"

// In thông tin user
void printUserInfo(const std::shared_ptr<UserAccount>& user);

// Khởi tạo dữ liệu mẫu (nếu cần mở rộng CLI)
void createSampleUser(DataManager& manager);

// Hàm đăng nhập giả lập
bool login(DataManager& manager, const std::string& username, const std::string& password);
