#pragma once

#include "data_manager.h"
#include <memory>

void showMainMenu();
void handleUserInput(DataManager& manager);

// Đăng ký và đăng nhập
void registerUser(DataManager& manager);
std::shared_ptr<UserAccount> loginUser(DataManager& manager);

// Sau đăng nhập
void showUserMenu(const std::shared_ptr<UserAccount>& user, DataManager& manager);
void handleUserActions(const std::shared_ptr<UserAccount>& user, DataManager& manager);

// Admin-only
void listUsers(DataManager& manager);
void deleteUser(DataManager& manager);
void createMultipleUsers(DataManager& manager);

// User-only
void changePassword(std::shared_ptr<UserAccount> user, DataManager& manager);
void showBalance(const std::shared_ptr<UserAccount>& user);
void transferPointsCLI(std::shared_ptr<UserAccount> sender, DataManager& manager);
void viewTransactionHistory(const std::shared_ptr<UserAccount>& user);
