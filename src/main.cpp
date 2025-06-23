#include "main.h"
#include "cli.h"
#include "data_manager.h"
#include "user_account.h"

#include <iostream>
#include <memory>

// Hàm tạo tài khoản admin mặc định nếu chưa có
void ensureDefaultAdmin(DataManager& manager) {
    std::string defaultAdminUsername = "admin";

    // Kiểm tra xem admin đã tồn tại chưa
    auto existingAdmin = manager.findUser(defaultAdminUsername);
    if (existingAdmin) {
        std::cout << "[✓] Default admin already exists.\n";
        return;
    }

    // Nếu chưa tồn tại, tạo mới
    std::string password = "Admin@123";
    auto admin = std::make_shared<UserAccount>(
        "Administrator",
        "admin@example.com",
        "000000000",
        defaultAdminUsername,
        password
    );
    admin->setRole(UserRole::ADMIN);
    admin->setStatus(AccountStatus::ACTIVE);
    admin->setTempPassword(true);  // buộc đổi mật khẩu lần đầu

    manager.saveUser(admin);

    std::cout << "[+] Default admin account created.\n";
    std::cout << "    Username: admin\n";
    std::cout << "    Password: " << password << "\n";
}

int main() {
    DataManager manager;

    // Tạo admin mặc định nếu chưa tồn tại
    ensureDefaultAdmin(manager);

    // Chạy giao diện người dùng
    handleUserInput(manager);

    return 0;
}
