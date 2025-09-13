
#include "adapters/photoneo/PhoxiDevice.h"
#include <mutex>

namespace avs { namespace adapters { namespace photoneo {

struct PhoxiDevice::Impl {
#ifdef WITH_PHOTONEO
    // Including Photoneo API only when WITH_PHOTONEO is ON
    #include <PhoXi.h>
    pho::api::PhoXiFactory factory;
    pho::api::PPhoXi scanner; // smart handle
#else
    // Stub placeholders
#endif
    std::mutex mtx; // protect scanner across calls
    std::string serial;
};

PhoxiDevice::PhoxiDevice() : impl_(new Impl()) {}
PhoxiDevice::~PhoxiDevice() { close(); }
PhoxiDevice::PhoxiDevice(PhoxiDevice&& other) noexcept = default;
PhoxiDevice& PhoxiDevice::operator=(PhoxiDevice&& other) noexcept = default;

Result PhoxiDevice::open(const std::string& serial, std::chrono::milliseconds timeout) {
    impl_->serial = serial;
#ifdef WITH_PHOTONEO
    std::lock_guard<std::mutex> lock(impl_->mtx);
    if (!impl_->factory.isPhoXiControlRunning()) {
        return Result::failure("PhoXi Control is not running");
    }
    // Try to connect
    impl_->scanner.Reset();
    impl_->scanner = impl_->factory.CreateAndConnect(serial, static_cast<int>(timeout.count()));
    if (!impl_->scanner || !impl_->scanner->isConnected()) {
        return Result::failure("Failed to connect to PhoXi device: " + serial);
    }
    return Result::success();
#else
    (void)timeout;
    return Result::failure("AVS built without Photoneo support (WITH_PHOTONEO=OFF)");
#endif
}

bool PhoxiDevice::isOpen() const noexcept {
#ifdef WITH_PHOTONEO
    std::lock_guard<std::mutex> lock(impl_->mtx);
    return impl_->scanner && impl_->scanner->isConnected();
#else
    return false;
#endif
}

void PhoxiDevice::close() {
#ifdef WITH_PHOTONEO
    std::lock_guard<std::mutex> lock(impl_->mtx);
    if (impl_->scanner && impl_->scanner->isConnected()) {
        try {
            if (impl_->scanner->isAcquiring()) impl_->scanner->StopAcquisition();
            impl_->scanner->Disconnect(false);
        } catch (...) {
            // swallow
        }
    }
    impl_->scanner.Reset();
#endif
}

Result PhoxiDevice::startAcquisition() {
#ifdef WITH_PHOTONEO
    std::lock_guard<std::mutex> lock(impl_->mtx);
    if (!impl_->scanner || !impl_->scanner->isConnected()) {
        return Result::failure("PhoXi device is not connected");
    }
    if (impl_->scanner->isAcquiring()) return Result::success();
    impl_->scanner->TriggerMode = pho::api::PhoXiTriggerMode::Software;
    impl_->scanner->StartAcquisition();
    if (!impl_->scanner->isAcquiring()) {
        return Result::failure("Failed to start acquisition");
    }
    return Result::success();
#else
    return Result::failure("AVS built without Photoneo support (WITH_PHOTONEO=OFF)");
#endif
}

Result PhoxiDevice::stopAcquisition() {
#ifdef WITH_PHOTONEO
    std::lock_guard<std::mutex> lock(impl_->mtx);
    if (!impl_->scanner || !impl_->scanner->isConnected()) {
        return Result::failure("PhoXi device is not connected");
    }
    if (!impl_->scanner->isAcquiring()) return Result::success();
    impl_->scanner->StopAcquisition();
    if (impl_->scanner->isAcquiring()) {
        return Result::failure("Failed to stop acquisition");
    }
    return Result::success();
#else
    return Result::failure("AVS built without Photoneo support (WITH_PHOTONEO=OFF)");
#endif
}

Result PhoxiDevice::grabPointCloud(PointCloud& cloud,
                                   std::chrono::milliseconds frameTimeout,
                                   bool organized) {
#ifdef WITH_PHOTONEO
    std::lock_guard<std::mutex> lock(impl_->mtx);
    if (!impl_->scanner || !impl_->scanner->isConnected()) {
        return Result::failure("PhoXi device is not connected");
    }
    if (!impl_->scanner->isAcquiring()) {
        impl_->scanner->TriggerMode = pho::api::PhoXiTriggerMode::Software;
        impl_->scanner->StartAcquisition();
        if (!impl_->scanner->isAcquiring()) {
            return Result::failure("Failed to start acquisition");
        }
    }

    int frame_id = impl_->scanner->TriggerFrame(true, true);
    if (frame_id < 0) {
        std::string msg;
        switch (frame_id) {
            case -1: msg = "Trigger not accepted"; break;
            case -2: msg = "Device is not running"; break;
            case -3: msg = "Communication error"; break;
            case -4: msg = "WaitForGrabbingEnd not supported"; break;
            default: msg = "Unknown trigger error"; break;
        }
        return Result::failure("PhoXi TriggerFrame failed: " + msg);
    }

    auto frame = impl_->scanner->GetSpecificFrame(frame_id, static_cast<int>(frameTimeout.count()));
    if (!frame || !frame->Successful) {
        return Result::failure("Invalid or unsuccessful frame");
    }

    int w = frame->GetResolution().Width;
    int h = frame->GetResolution().Height;

    cloud.organized = organized;
    cloud.width = organized ? w : 0;
    cloud.height = organized ? h : 0;
    cloud.points.clear();
    cloud.points.reserve(static_cast<size_t>(w) * static_cast<size_t>(h));

    // Values appear in millimeters in PhoXi API samples; convert to meters.
    for (int r = 0; r < h; ++r) {
        for (int c = 0; c < w; ++c) {
            auto p = frame->PointCloud.At(r, c);
            if (!organized && p == pho::api::Point3_32f(0,0,0)) continue;
            Vec3f out{ p.x / 1000.0f, p.y / 1000.0f, p.z / 1000.0f };
            cloud.points.push_back(out);
        }
    }
    return Result::success();
#else
    (void)cloud; (void)frameTimeout; (void)organized;
    return Result::failure("AVS built without Photoneo support (WITH_PHOTONEO=OFF)");
#endif
}

std::vector<std::string> PhoxiDevice::listDevices(Result* err) {
#ifdef WITH_PHOTONEO
    try {
        Impl tmp; // local factory
        if (!tmp.factory.isPhoXiControlRunning()) {
            if (err) *err = Result::failure("PhoXi Control is not running");
            return {};
        }
        auto list = tmp.factory.GetDeviceList();
        std::vector<std::string> out;
        out.reserve(list.size());
        for (const auto& di : list) out.push_back(di.HardwareIdentification);
        if (err) *err = Result::success();
        return out;
    } catch (...) {
        if (err) *err = Result::failure("PhoXi API exception while listing devices");
        return {};
    }
#else
    if (err) *err = Result::failure("AVS built without Photoneo support (WITH_PHOTONEO=OFF)");
    return {};
#endif
}

}}} // namespace
