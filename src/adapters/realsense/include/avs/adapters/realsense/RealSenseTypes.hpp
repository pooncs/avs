#pragma once
#include <cstdint>
#include <array>
#include <string>
#include <vector>
#include <chrono>

namespace avs { namespace adapters { namespace realsense {

struct RSIntrinsics {
    int width{0};
    int height{0};
    float fx{0}, fy{0}, cx{0}, cy{0};
    std::array<float,5> dist{{0,0,0,0,0}}; // k1,k2,p1,p2,k3
};

enum class RSStream { Color, Depth };

struct RSFrame {
    RSStream stream{RSStream::Color};
    int width{0};
    int height{0};
    int stride{0};
    int bpp{0};
    std::vector<uint8_t> data; // Depth stored as little-endian 16-bit
    RSIntrinsics intr;         // Matched intrinsics
    std::chrono::steady_clock::time_point t;
};

}}} // namespace
