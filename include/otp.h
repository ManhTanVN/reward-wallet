#pragma once
#include <string>
#include <chrono>

/**
 * @class OTPManager
 * @brief Lớp tiện ích để xử lý mã xác thực OTP (One-Time Password).
 *
 * Lớp này cung cấp các hàm để sinh mã OTP ngẫu nhiên 6 chữ số và kiểm tra tính hợp lệ của OTP
 * dựa trên thời gian hết hạn và mã người dùng nhập vào.
 */
class OTPManager
{
public:
    /**
     * @brief Sinh mã OTP gồm 6 chữ số.
     *
     * Mỗi lần gọi sẽ tạo ra một chuỗi số ngẫu nhiên từ 100000 đến 999999.
     *
     * @return Mã OTP dưới dạng chuỗi.
     */
    static std::string generateOTP();

    /**
     * @brief Kiểm tra tính hợp lệ của mã OTP.
     *
     * So sánh mã OTP hệ thống sinh ra với đầu vào người dùng, đồng thời kiểm tra xem mã còn trong thời gian hiệu lực không.
     *
     * @param otp Mã OTP đã sinh ra và lưu trong hệ thống.
     * @param input Mã OTP người dùng nhập vào.
     * @param expiry Thời gian hết hạn của OTP.
     * @return true nếu OTP hợp lệ và còn hiệu lực; ngược lại trả về false.
     */
    static bool isOTPValid(const std::string &otp,
                           const std::string &input,
                           const std::chrono::system_clock::time_point &expiry);
};
