# Reward Wallet

## 🔍 Giới thiệu dự án
**Reward Wallet** là một hệ thống ví thưởng đơn giản viết bằng C++. Dự án hỗ trợ đăng ký người dùng, đăng nhập, phân quyền (admin/user), gửi điểm (points), mua điểm, và quản lý lịch sử giao dịch.

## 👥 Thành viên và phân công công việc
- **Nguyễn Văn Tân** - Lập trình chính
  - Thiết kế cấu trúc hệ thống
  - Hiện thực lớp `UserAccount`, `DataManager`, `Utils`
- **Nguyễn Cảnh Tuấn** - Lập trình chính
  - Thiết kế user interface
  - Hiện thực lớp `CLI`, `Main`
- **Nguyễn Thế Linh** - Lập trình chính
  - Hiện thực lớp `UserAccount`, `Sha256`
  - Xây dựng giao diện dòng lệnh và xử lý OTP, phân quyền
- **Nguyễn Thanh Thủy** - Lập trình chính
  - Hiện thực lớp `Wallet`, `Utils`
  - Kiểm thử và viết lại các comments

## 📋 Phân tích & đặc tả chức năng

### 1. Quản lý người dùng
- Đăng ký người dùng mới với tên, email, username, password
- Phân quyền: Admin hoặc User
- Mỗi người dùng có một địa chỉ ví riêng

### 2. Đăng nhập
- Xác thực bằng username/password
- Hỗ trợ OTP khi đăng nhập (nếu được bật)
- Nếu dùng mật khẩu tạm => buộc đổi mật khẩu

### 3. Mua điểm (Buy points)
- Xác minh lại mật khẩu + OTP
- Sinh điểm mới và cộng vào ví người dùng
- Ghi log kiểu `"type": "buy"`

### 4. Gửi điểm (Transfer points)
- Gửi điểm giữa các ví người dùng
- Xác thực OTP (trừ master wallet)
- Ghi log gửi/nhận kèm thời gian, note

### 5. Quản trị (Admin)
- Xem danh sách người dùng
- Xóa người dùng
- Tạo nhiều người dùng một lúc với mật khẩu tạm thời

### 6. Lịch sử giao dịch
- Hiển thị đầy đủ log điểm đã gửi/nhận
- Phân biệt rõ loại `"buy"` hoặc `"transfer"`

### 7. Tặng 1000 điểm cho 10 người đầu tiên
- Admin tặng 1000 điểm tự động từ ví `__master__wallet__` cho 10 người dùng đầu tiên

## 🧰 Hướng dẫn cài đặt & chạy

### Yêu cầu hệ thống
- Compiler hỗ trợ C++17 (g++, clang++, MSVC)
- CMake >= 3.10
- Thư viện: [nlohmann/json](https://github.com/nlohmann/json) (đã đính kèm)

### Tải mã nguồn
```bash
git clone https://github.com/ManhTanVN/reward-wallet.git
cd reward-wallet
```

### Biên dịch

1. **MacOs**

cmake --preset macos-clang 

cmake --build --preset macos-clang

**Run**:  ./build/macos-clang/output/reward-wallet

2. **Windows**

cmake --preset windows-mingw

cmake --build --preset windows-mingw

**Run**: build/windows-mingw/output/reward-wallet


### File dữ liệu
- `data/users.json`: lưu toàn bộ thông tin người dùng (được đọc/ghi qua lớp `DataManager`)

## 📂 Cấu trúc thư mục

```
reward-wallet/
├── include/
│   ├── auth.h
│   ├── cli.h
│   ├── data_manager.h
│   ├── user_account.h
│   ├── wallet.h
│   ├── otp.h
│   ├── sha256.h
│   ├── main.h
├── src/
│   ├── auth.cpp
│   ├── cli.cpp
│   ├── data-manager.cpp
│   ├── user_account.cpp
│   ├── wallet.cpp
│   ├── sha256.cpp
│   ├── main.cpp
│   ├── otp.cpp
├── data/
│   └── users.json
├── CMakeLists.txt
└── README.md
```

## 📚 Tài liệu tham khảo
- [nlohmann/json](https://github.com/nlohmann/json) - thư viện JSON cho C++
- StackOverflow - các giải pháp về `std::chrono`, `std::shared_ptr`, `OTP logic`

## 📝 Ghi chú về mã nguồn
- Toàn bộ mã nguồn có chú thích rõ ràng.
- Các hàm được viết theo nguyên tắc: rõ ràng, dễ hiểu, tách biệt logic giao diện và xử lý dữ liệu.
- Có xử lý lỗi (try/catch) trong các phần nhạy cảm như đọc log, OTP, xác thực.

---





# reward-wallet

# install cmake
⚙️ How to build using CMake in VSCode (Windows):
Install CMake & Compiler

Install CMake

Install MSVC via Visual Studio or install MinGW

Install extensions in VS Code

CMake Tools

C/C++ (by Microsoft)

Configure project
Open VS Code in your project folder and hit:

Ctrl + Shift + P → type CMake: Configure

Then Ctrl + Shift + P → CMake: Build

# make sure these configs below are included in setting.json

  "cmake.useCMakePresets": "always",
  "cmake.preferredGenerators": ["MinGW Makefiles"],
  "cmake.configureOnOpen": true,
  "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools"



