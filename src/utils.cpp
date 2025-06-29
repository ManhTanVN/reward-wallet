#include "utils.h"

#include <chrono>     // Để làm việc với thời gian (std::chrono)
#include <ctime>      // Chuyển đổi thời gian về định dạng std::time_t
#include <sstream>    // Dùng để xây dựng chuỗi (std::stringstream)
#include <iomanip>    // Định dạng đầu ra thời gian (std::put_time)
#include <iostream>   // Nhập xuất chuẩn
#include <limits>     // Để đặt giới hạn khi xử lý nhập liệu
#include <filesystem> // Làm việc với file và thư mục
#include <fstream>    // Đọc ghi file
#include <vector>     // Dùng vector để lưu danh sách backup
#include <algorithm>  // Dùng sort

namespace fs = std::filesystem; // Alias ngắn gọn cho std::filesystem

// Trả về timestamp hiện tại ở định dạng "YYYY-MM-DD HH:MM:SS"
std::string currentTimestamp()
{
    auto now = std::chrono::system_clock::now();                   // Lấy thời điểm hiện tại
    std::time_t timeT = std::chrono::system_clock::to_time_t(now); // Chuyển sang time_t
    std::stringstream ss;
    ss << std::put_time(std::localtime(&timeT), "%Y-%m-%d %H:%M:%S"); // Format thời gian
    return ss.str();
}

// Hàm nhập số nguyên có kiểm tra phạm vi và lỗi nhập liệu
int getValidatedInput(const std::string &prompt, int min, int max)
{
    int choice;
    while (true)
    {
        std::cout << prompt;
        std::cin >> choice;

        // Nếu nhập không phải số
        if (std::cin.fail())
        {
            std::cin.clear();                                                   // Xóa trạng thái lỗi
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Bỏ qua phần còn lại của dòng
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        // Bỏ phần dư còn lại trên dòng nhập
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Kiểm tra nếu trong phạm vi hợp lệ
        if (choice >= min && choice <= max)
        {
            return choice;
        }
        else
        {
            std::cout << "Please enter a number between " << min << " and " << max << ".\n";
        }
    }
}

// Hàm sao lưu file nguồn sang thư mục backup
void createBackup(const std::string &sourceFile, const std::string &backupDir)
{
    try
    {
        // Nếu thư mục backup chưa tồn tại thì tạo
        if (!fs::exists(backupDir))
        {
            fs::create_directories(backupDir);
        }

        // Tạo tên file backup với timestamp
        std::string backupName = "backup_" + currentTimestamp() + ".json";

        // Đổi dấu ':' thành '-' vì Windows không cho phép ':' trong tên file
        std::replace(backupName.begin(), backupName.end(), ':', '-');

        // Đường dẫn đầy đủ đến file backup
        fs::path targetPath = fs::path(backupDir) / backupName;

        // Sao chép file nguồn sang file backup
        fs::copy_file(sourceFile, targetPath, fs::copy_options::overwrite_existing);
        std::cout << "[Backup] Created: " << targetPath << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Backup ERROR] " << e.what() << "\n";
    }
}

// Hàm khôi phục từ file backup
void restoreFromBackup(const std::string &backupFile, const std::string &targetFile)
{
    try
    {
        // Nếu file backup không tồn tại
        if (!fs::exists(backupFile))
        {
            std::cerr << "[Restore] Backup file not found: " << backupFile << "\n";
            return;
        }

        // Ghi đè nội dung file đích bằng nội dung từ file backup
        fs::copy_file(backupFile, targetFile, fs::copy_options::overwrite_existing);
        std::cout << "[Restore] Restored from " << backupFile << " to " << targetFile << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Restore ERROR] " << e.what() << "\n";
    }
}

// Hiển thị danh sách các file backup hiện có và cho người dùng chọn để khôi phục
void listBackupFiles(const std::string &backupDir)
{
    try
    {
        // Nếu thư mục backup chưa tồn tại
        if (!fs::exists(backupDir))
        {
            std::cout << "[Backup] No backup directory found.\n";
            return;
        }

        std::vector<std::string> backups;

        // Duyệt thư mục và lưu tên các file vào vector
        for (const auto &entry : fs::directory_iterator(backupDir))
        {
            if (entry.is_regular_file())
            {
                backups.push_back(entry.path().string());
            }
        }

        // Nếu không có file backup nào
        if (backups.empty())
        {
            std::cout << "[Backup] No backup files found.\n";
            return;
        }

        // Sắp xếp danh sách file backup theo thứ tự mới nhất
        std::sort(backups.begin(), backups.end(), std::greater<>());

        std::cout << "Available backups:\n";
        for (size_t i = 0; i < backups.size(); ++i)
        {
            std::cout << "  [" << i << "] " << fs::path(backups[i]).filename().string() << "\n";
        }

        // Nhập chỉ số backup để chọn
        int index = getValidatedInput("Choose backup index to restore: ", 0, backups.size() - 1);

        // Thực hiện khôi phục file được chọn
        restoreFromBackup(backups[index], "data/users.json");
        std::cout << "[SUCCESS] Backup restored successfully.\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "[Backup Listing ERROR] " << e.what() << "\n";
    }
}
