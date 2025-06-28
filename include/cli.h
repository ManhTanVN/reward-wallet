#pragma once

#include "data_manager.h"
#include "user_account.h"  // Cần cho std::shared_ptr<UserAccount>
#include <memory>

// Giao diện chính
void showMainMenu();
void handleUserInput(DataManager& manager);

// Đăng ký và đăng nhập
void registerUser(DataManager& manager);
std::shared_ptr<UserAccount> loginUser(DataManager& manager);

// Menu sau khi đăng nhập
void showUserMenu(const std::shared_ptr<UserAccount>& user, DataManager& manager);

// Các thao tác dành riêng cho admin
void listUsers(DataManager& manager);
void deleteUser(DataManager& manager);
void createMultipleUsers(DataManager& manager);

// Các thao tác dành cho user bình thường
void changePassword(std::shared_ptr<UserAccount> user, DataManager& manager);
void showBalance(const std::shared_ptr<UserAccount>& user);
void transferPointsCLI(std::shared_ptr<UserAccount> sender, DataManager& manager);
void viewTransactionHistory(const std::shared_ptr<UserAccount>& user);
