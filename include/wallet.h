#pragma once // Tránh include file này nhiều lần khi biên dịch

#include "data_manager.h" // Cần thiết để truy cập, sửa dữ liệu người dùng
#include <string>         // Sử dụng std::string cho tham số và xử lý chuỗi

// Hàm chuyển điểm từ ví người dùng này sang người dùng khác
// - manager: đối tượng quản lý dữ liệu người dùng (đọc/ghi JSON)
// - fromWallet: địa chỉ ví của người gửi
// - toWallet: địa chỉ ví của người nhận
// - amount: số lượng điểm muốn chuyển
// - note: ghi chú cho giao dịch  (mặc định rỗng nếu không cung cấp)
//
// Trả về true nếu chuyển thành công, false nếu có lỗi (ví dụ: không đủ điểm, OTP sai,...)
bool transferPoints(DataManager &manager,
                    const std::string &fromWallet,
                    const std::string &toWallet,
                    int amount,
                    const std::string &note = "");
