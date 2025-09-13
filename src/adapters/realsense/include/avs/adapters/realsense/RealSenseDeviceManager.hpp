#pragma once
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include "RealSenseTypes.hpp"

namespace avs { namespace adapters { namespace realsense {

struct DeviceInfo { std::string serial; std::string name; };

class RealSenseDeviceManager {
public:
    RealSenseDeviceManager();
    ~RealSenseDeviceManager();

    RealSenseDeviceManager(const RealSenseDeviceManager&) = delete;
    RealSenseDeviceManager& operator=(const RealSenseDeviceManager&) = delete;

    std::vector<DeviceInfo> list() const; // empty if WITH_REALSENSE=OFF
    bool open(const std::string& serial="");
    void close();
    bool isOpen() const;
    void* raw() const; // opaque handle

private:
    struct Impl; std::unique_ptr<Impl> impl; // pImpl
};

}}}
