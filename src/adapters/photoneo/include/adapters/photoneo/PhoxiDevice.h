
#pragma once

// Photoneo PhoXi adapter
// RAII wrapper around PhoXi API to connect and grab a point cloud.
// Comments intentionally brief; see docs/ARCHITECTURE.md for overview.

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <memory>

namespace avs { namespace adapters { namespace photoneo {

struct Vec3f {
    float x{}, y{}, z{};
};

struct PointCloud {
    std::vector<Vec3f> points;   // meters
    int width{0};                // if organized
    int height{0};               // if organized
    bool organized{false};
};

struct Result {
    bool ok{true};
    std::string msg;
    static Result success() { return {true, {}}; }
    static Result failure(std::string m) { return {false, std::move(m)}; }
};

class PhoxiDevice {
public:
    PhoxiDevice();
    ~PhoxiDevice();
    PhoxiDevice(PhoxiDevice&&) noexcept;
    PhoxiDevice& operator=(PhoxiDevice&&) noexcept;
    PhoxiDevice(const PhoxiDevice&) = delete;
    PhoxiDevice& operator=(const PhoxiDevice&) = delete;

    // Open device by hardware identification (serial). Requires PhoXi Control running.
    Result open(const std::string& serial, std::chrono::milliseconds timeout = std::chrono::milliseconds(5000));
    bool isOpen() const noexcept;
    void close();

    Result startAcquisition();
    Result stopAcquisition();

    // Grab a frame and fill cloud (meters). If organized=true, width/height set accordingly.
    Result grabPointCloud(PointCloud& cloud,
                          std::chrono::milliseconds frameTimeout = std::chrono::milliseconds(10000),
                          bool organized = true);

    // Enumerate available PhoXi devices (HardwareIdentification strings).
    static std::vector<std::string> listDevices(Result* err = nullptr);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}}} // namespace avs::adapters::photoneo
