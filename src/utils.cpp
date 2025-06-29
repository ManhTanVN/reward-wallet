#include "utils.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <filesystem>
#include <fstream>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

std::string currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t timeT = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

int getValidatedInput(const std::string& prompt, int min, int max) {
    int choice;
    while (true) {
        std::cout << prompt;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice >= min && choice <= max) {
            return choice;
        } else {
            std::cout << "Please enter a number between " << min << " and " << max << ".\n";
        }
    }
}

void createBackup(const std::string& sourceFile, const std::string& backupDir) {
    try {
        if (!fs::exists(backupDir)) {
            fs::create_directories(backupDir);  // ✅ Tạo thư mục nếu chưa có
        }

        std::string backupName = "backup_" + currentTimestamp() + ".json";
        std::replace(backupName.begin(), backupName.end(), ':', '-');  // Windows không cho phép dấu :
        fs::path targetPath = fs::path(backupDir) / backupName;

        fs::copy_file(sourceFile, targetPath, fs::copy_options::overwrite_existing);
        std::cout << "[Backup] Created: " << targetPath << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[Backup ERROR] " << e.what() << "\n";
    }
}


void restoreFromBackup(const std::string& backupFile, const std::string& targetFile) {
    try {
        if (!fs::exists(backupFile)) {
            std::cerr << "[Restore] Backup file not found: " << backupFile << "\n";
            return;
        }

        fs::copy_file(backupFile, targetFile, fs::copy_options::overwrite_existing);
        std::cout << "[Restore] Restored from " << backupFile << " to " << targetFile << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[Restore ERROR] " << e.what() << "\n";
    }
}

void listBackupFiles(const std::string& backupDir) {
    try {
        if (!fs::exists(backupDir)) {
            std::cout << "[Backup] No backup directory found.\n";
            return;
        }

        std::vector<std::string> backups;
        for (const auto& entry : fs::directory_iterator(backupDir)) {
            if (entry.is_regular_file()) {
                backups.push_back(entry.path().string());
            }
        }

        if (backups.empty()) {
            std::cout << "[Backup] No backup files found.\n";
            return;
        }
        std::sort(backups.begin(), backups.end(), std::greater<>());
        std::cout << "Available backups:\n";
        for (size_t i = 0; i < backups.size(); ++i) {
            std::cout << "  [" << i << "] " << fs::path(backups[i]).filename().string() << "\n";
        }

        int index = getValidatedInput("Choose backup index to restore: ", 0, backups.size() - 1);
        restoreFromBackup(backups[index], "data/users.json");
        std::cout << "[SUCCESS] Backup restored successfully.\n";

    } catch (const std::exception& e) {
        std::cerr << "[Backup Listing ERROR] " << e.what() << "\n";
    }
}
