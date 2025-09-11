#include "otp.h"
#include <random>
#include <ctime>

/**
 * Sinh mã OTP ngẫu nhiên 6 chữ số.
 *
 * Sử dụng generator `std::mt19937` và phân phối đều `std::uniform_int_distribution`
 * để tạo số nguyên ngẫu nhiên trong khoảng từ 100000 đến 999999.
 *
 * Chuỗi OTP gồm 6 chữ số (string).
 */
std::string OTPManager::generateOTP()
{
    std::random_device rd;                                // Dùng để lấy seed ngẫu nhiên từ phần cứng
    std::mt19937 gen(rd());                               // Tạo bộ sinh số ngẫu nhiên theo chuẩn Mersenne Twister
    std::uniform_int_distribution<> dist(100000, 999999); // Phân phối đều số trong khoảng 6 chữ số
    return std::to_string(dist(gen));                     // Trả về chuỗi số OTP
}

/**
 * Kiểm tra tính hợp lệ của OTP.
 *
 * So sánh mã OTP được sinh ra với đầu vào của người dùng, đồng thời kiểm tra hạn sử dụng (expiry).
 *
 * otp Mã OTP hệ thống sinh ra.
 * input Mã OTP người dùng nhập vào.
 * expiry Thời điểm hết hạn của OTP.
 * true nếu hợp lệ (trùng OTP và chưa hết hạn), false nếu sai mã hoặc hết hạn.
 */
bool OTPManager::isOTPValid(const std::string &otp,
                            const std::string &input,
                            const std::chrono::system_clock::time_point &expiry)
{
    auto now = std::chrono::system_clock::now(); // Lấy thời điểm hiện tại
    return (now < expiry) && (otp == input);     // Kiểm tra còn hạn và khớp mã
}