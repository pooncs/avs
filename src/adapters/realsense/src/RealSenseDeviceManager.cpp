#include "avs/adapters/realsense/RealSenseDeviceManager.hpp"
#include <mutex>

#if defined(WITH_REALSENSE)
#  include <librealsense2/rs.hpp>
#endif

namespace avs { namespace adapters { namespace realsense {

struct RealSenseDeviceManager::Impl {
#if defined(WITH_REALSENSE)
    rs2::context ctx;
    rs2::device dev;
#endif
    bool open{false};
    mutable std::mutex mtx;
};

RealSenseDeviceManager::RealSenseDeviceManager() : impl(new Impl) {}
RealSenseDeviceManager::~RealSenseDeviceManager() { close(); }

std::vector<DeviceInfo> RealSenseDeviceManager::list() const {
    std::lock_guard<std::mutex> lk(impl->mtx);
    std::vector<DeviceInfo> out;
#if defined(WITH_REALSENSE)
    for (auto&& d : impl->ctx.query_devices()) {
        DeviceInfo i; i.serial = d.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER); i.name = d.get_info(RS2_CAMERA_INFO_NAME); out.push_back(i);
    }
#endif
    return out;
}

bool RealSenseDeviceManager::open(const std::string& serial) {
    std::lock_guard<std::mutex> lk(impl->mtx);
#if defined(WITH_REALSENSE)
    for (auto&& d : impl->ctx.query_devices()) {
        std::string s = d.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER);
        if (serial.empty() || s==serial) { impl->dev = d; impl->open = true; return true; }
    }
    return false;
#else
    (void)serial; impl->open = true; return true; // stub
#endif
}

void RealSenseDeviceManager::close() {
    std::lock_guard<std::mutex> lk(impl->mtx);
#if defined(WITH_REALSENSE)
    impl->dev = rs2::device();
#endif
    impl->open = false;
}

bool RealSenseDeviceManager::isOpen() const { std::lock_guard<std::mutex> lk(impl->mtx); return impl->open; }

void* RealSenseDeviceManager::raw() const { return nullptr; }

}}}
