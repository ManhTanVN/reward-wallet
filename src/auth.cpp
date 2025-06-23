#include "auth.h"
#include <iostream>

std::shared_ptr<UserAccount> authenticateUser(DataManager& manager, const std::string& username, const std::string& password) {
    auto user = manager.findUser(username);
    if (user && user->validatePassword(password)) {
        return user;
    }
    return nullptr;
}

bool requirePasswordChange(std::shared_ptr<UserAccount> user) {
    return user->isUsingTempPassword();
}

// void changePassword(std::shared_ptr<UserAccount> user, DataManager& manager) {
//     std::string newPassword;
//     std::cout << "Enter new password: ";
//     std::getline(std::cin, newPassword);

//     if (!UserAccount::isPasswordValid(newPassword)) {
//         std::cout << "Weak password. Must contain upper, lower, digit, special character, and ≥8 characters.\n";
//         return;
//     }

//     user->setPassword(newPassword);
//     user->setTempPassword(false);
//     manager.saveUser(user);
//     std::cout << "Password changed successfully.\n";
// }
