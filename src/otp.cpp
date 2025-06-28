#include "otp.h"
#include <random>
#include <ctime>

std::string OTPManager::generateOTP() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(100000, 999999);
    return std::to_string(dist(gen));
}

bool OTPManager::isOTPValid(const std::string& otp, const std::string& input, const std::chrono::system_clock::time_point& expiry) {
    auto now = std::chrono::system_clock::now();
    return (now < expiry) && (otp == input);
}
