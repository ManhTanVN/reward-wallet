#pragma once

#include "data_manager.h" // Quản lý lưu trữ và truy xuất thông tin người dùng
#include "user_account.h" // Định nghĩa lớp UserAccount, chứa thông tin và chức năng xác thực
#include <memory>         // Sử dụng std::shared_ptr để quản lý bộ nhớ người dùng

// Giao diện chính cho người dùng chưa đăng nhập
void showMainMenu();                        // Hiển thị menu chính
void handleUserInput(DataManager &manager); // Xử lý lựa chọn của người dùng từ menu chính

// Chức năng đăng ký và đăng nhập
void registerUser(DataManager &manager);                      // Đăng ký tài khoản mới
std::shared_ptr<UserAccount> loginUser(DataManager &manager); // Đăng nhập và trả về người dùng đã xác thực

// Giao diện và chức năng sau khi đăng nhập
void showUserMenu(const std::shared_ptr<UserAccount> &user, DataManager &manager); // Hiển thị menu cá nhân hóa sau khi đăng nhập

// Các thao tác dành riêng cho người dùng có vai trò quản trị (admin)
void listUsers(DataManager &manager);           // Liệt kê tất cả người dùng
void deleteUser(DataManager &manager);          // Xóa người dùng theo tên đăng nhập
void createMultipleUsers(DataManager &manager); // Tạo nhiều người dùng một cách tự động (dành cho admin)

// Các thao tác dành cho người dùng thông thường (normal user)
void changePassword(std::shared_ptr<UserAccount> user, DataManager &manager);                // Đổi mật khẩu
void showBalance(const std::shared_ptr<UserAccount> &user, DataManager &manager);            // Xem số dư điểm trong ví
void transferPointsCLI(std::shared_ptr<UserAccount> sender, DataManager &manager);           // Chuyển điểm đến ví khác
void viewTransactionHistory(const std::shared_ptr<UserAccount> &user, DataManager &manager); // Xem lịch sử giao dịch
