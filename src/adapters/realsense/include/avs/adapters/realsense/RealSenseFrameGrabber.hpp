#pragma once
#include <memory>
#include <string>
#include <optional>
#include "RealSenseTypes.hpp"
#include "RealSenseDeviceManager.hpp"

namespace avs { namespace adapters { namespace realsense {

struct RSConfig {
    int color_w{1280};
    int color_h{720};
    int depth_w{640};
    int depth_h{480};
    int fps{30};
    bool enable_color{true};
    bool enable_depth{true};
};

class RealSenseFrameGrabber {
public:
    explicit RealSenseFrameGrabber(std::shared_ptr<RealSenseDeviceManager> dev);
    ~RealSenseFrameGrabber();

    RealSenseFrameGrabber(const RealSenseFrameGrabber&) = delete;
    RealSenseFrameGrabber& operator=(const RealSenseFrameGrabber&) = delete;

    bool start(const RSConfig& cfg);
    void stop();
    bool running() const;

    std::optional<RSFrame> getColor();
    std::optional<RSFrame> getDepth();

    RSIntrinsics colorIntrinsics() const; // zeroed if unavailable
    RSIntrinsics depthIntrinsics() const; // zeroed if unavailable

private:
    struct Impl; std::unique_ptr<Impl> impl;
};

}}}
