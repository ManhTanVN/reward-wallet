#pragma once

#include <string>

// Lấy thời gian hiện tại định dạng "YYYY-MM-DD HH:MM:SS"
std::string currentTimestamp();

// Lấy input nguyên hợp lệ từ người dùng trong khoảng [min, max]
int getValidatedInput(const std::string& prompt, int min, int max);

// Tạo bản sao lưu (backup) file nguồn vào thư mục backupDir
void createBackup(const std::string& sourceFile, const std::string& backupDir);

// Khôi phục dữ liệu từ file backup vào file đích
void restoreFromBackup(const std::string& backupFile, const std::string& targetFile);

// Hiển thị danh sách các file backup có trong thư mục backupDir và cho phép chọn để khôi phục
void listBackupFiles(const std::string& backupDir);
