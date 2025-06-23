#include "sha256.h"
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>

// Thuật toán SHA256 rút gọn từ thư viện open-source (cấp phép MIT)
// Bạn có thể dùng các phiên bản an toàn hơn nếu dùng cho sản phẩm thực tế.

#include <cstdint>
#include <array>

namespace
{
    constexpr std::array<uint32_t, 64> k = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
        0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
        0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
        0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
        0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
        0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
        0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
        0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    inline uint32_t rotr(uint32_t x, uint32_t n)
    {
        return (x >> n) | (x << (32 - n));
    }

    inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z)
    {
        return (x & y) ^ (~x & z);
    }

    inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z)
    {
        return (x & y) ^ (x & z) ^ (y & z);
    }

    inline uint32_t big_sigma0(uint32_t x)
    {
        return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
    }

    inline uint32_t big_sigma1(uint32_t x)
    {
        return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
    }

    inline uint32_t small_sigma0(uint32_t x)
    {
        return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
    }

    inline uint32_t small_sigma1(uint32_t x)
    {
        return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
    }
}

std::string sha256(const std::string &input)
{
    uint64_t l = input.size() * 8;
    std::vector<uint8_t> data(input.begin(), input.end());
    data.push_back(0x80);
    while ((data.size() + 8) % 64 != 0)
        data.push_back(0x00);

    for (int i = 7; i >= 0; --i)
        data.push_back((l >> (i * 8)) & 0xff);

    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372,
        0xa54ff53a, 0x510e527f, 0x9b05688c,
        0x1f83d9ab, 0x5be0cd19};

    for (size_t i = 0; i < data.size(); i += 64)
    {
        uint32_t w[64];
        for (int t = 0; t < 16; ++t)
        {
            w[t] = (data[i + t * 4] << 24) |
                   (data[i + t * 4 + 1] << 16) |
                   (data[i + t * 4 + 2] << 8) |
                   (data[i + t * 4 + 3]);
        }
        for (int t = 16; t < 64; ++t)
        {
            w[t] = small_sigma1(w[t - 2]) + w[t - 7] +
                   small_sigma0(w[t - 15]) + w[t - 16];
        }

        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], hval = h[7];

        for (int t = 0; t < 64; ++t)
        {
            uint32_t t1 = hval + big_sigma1(e) + ch(e, f, g) + k[t] + w[t];
            uint32_t t2 = big_sigma0(a) + maj(a, b, c);
            hval = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
        h[5] += f;
        h[6] += g;
        h[7] += hval;
    }

    std::stringstream ss;
    for (int i = 0; i < 8; ++i)
        ss << std::hex << std::setw(8) << std::setfill('0') << h[i];

    return ss.str();
}
