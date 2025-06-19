#include "cli.h"
#include <iostream>
#include <limits>

void showMenu()
{
    std::cout << "\n========= User Management CLI =========\n";
    std::cout << "1. Register new user\n";
    std::cout << "2. Login\n";
    std::cout << "3. List all users\n";
    std::cout << "4. Delete user\n";
    std::cout << "0. Exit\n";
    std::cout << "=======================================\n";
    std::cout << "Enter your choice: ";
}

void handleUserInput(DataManager &manager)
{
    int choice;
    do
    {
        showMenu();
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear input buffer

        switch (choice)
        {
        case 1:
            registerUser(manager);
            break;
        case 2:
            loginUser(manager);
            break;
        case 3:
            listUsers(manager);
            break;
        case 4:
            deleteUser(manager);
            break;
        case 0:
            std::cout << "Goodbye!\n";
            break;
        default:
            std::cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);
}

void registerUser(DataManager &manager)
{
    std::string name, email, id, username, password;

    std::cout << "Full Name: ";
    std::getline(std::cin, name);

    std::cout << "Email: ";
    std::getline(std::cin, email);

    std::cout << "ID Number: ";
    std::getline(std::cin, id);

    std::cout << "Username: ";
    std::getline(std::cin, username);

    std::cout << "Password: ";
    std::getline(std::cin, password);

    if (!UserAccount::isPasswordValid(password))
    {
        std::cout << "Weak password. Must contain upper, lower, digit, special character, and ≥8 characters.\n";
        return;
    }

    auto user = std::make_shared<UserAccount>(name, email, id, username, password);
    manager.saveUser(user);
    std::cout << "User registered successfully.\n";
}

void loginUser(DataManager &manager)
{
    std::string username, password;
    std::cout << "Username: ";
    std::getline(std::cin, username);

    std::cout << "Password: ";
    std::getline(std::cin, password);

    auto users = manager.loadAllUsers();
    for (const auto &u : users)
    {
        if (u->getUsername() == username && u->validatePassword(password))
        {
            std::cout << "Login successful!\n";
            return;
        }
    }

    std::cout << "Login failed. Invalid username or password.\n";
}

void listUsers(DataManager &manager)
{
    auto users = manager.loadAllUsers();
    if (users.empty())
    {
        std::cout << "No users found.\n";
        return;
    }

    for (const auto &u : users)
    {
        std::cout << "- " << u->getUsername() << " (" << u->getEmail() << ")\n";
    }
}

void deleteUser(DataManager &manager)
{
    std::string username;
    std::cout << "Enter username to delete: ";
    std::getline(std::cin, username);

    manager.removeUser(username);
    std::cout << "User deleted if existed.\n";
}
