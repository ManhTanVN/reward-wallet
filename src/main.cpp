#include "main.h"
#include "cli.h"
#include "data_manager.h"
#include "user_account.h"

#include <iostream>
#include <memory>

// Hàm tạo tài khoản admin mặc định nếu chưa có
void ensureDefaultAdmin(DataManager& manager) {
    const std::string defaultAdminUsername = "admin";

    auto existingAdmin = manager.findUser(defaultAdminUsername);
    if (existingAdmin) {
        std::cout << "[✓] Default admin already exists.\n";
        return;
    }

    std::string password = "Admin@123";
    auto admin = std::make_shared<UserAccount>(
        "Administrator",
        "admin@example.com",
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

// Hàm tạo ví tổng nếu chưa có
void ensureMasterWalletExists(DataManager& manager) {
    const std::string masterUsername = "__master__wallet__";

    if (manager.findUser(masterUsername)) {
        std::cout << "Master wallet already exists.\n";
        return;
    }

    auto master = std::make_shared<UserAccount>(
        "System Wallet",
        "system@wallet.com",
        masterUsername,
        "Master@123!"
    );
    master->setRole(UserRole::ADMIN);
    master->setPointBalance(10000);    // điểm gốc
    master->setStatus(AccountStatus::ACTIVE);

    manager.saveUser(master);

    std::cout << "[Init] Master wallet created with 10,000 points.\n";
}

int main() {
    DataManager manager;

    ensureDefaultAdmin(manager);
    ensureMasterWalletExists(manager);

    handleUserInput(manager);

    return 0;
}
