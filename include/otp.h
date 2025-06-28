#pragma once
#include <string>
#include <chrono>

class OTPManager {
public:
    static std::string generateOTP();
    static bool isOTPValid(const std::string& otp, const std::string& input, const std::chrono::system_clock::time_point& expiry);
};
