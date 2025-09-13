#pragma once
#include <memory>
#include <string>
#include <optional>
#include <avs/adapters/realsense/RealSenseFrameGrabber.hpp>

namespace avs { namespace nodes { namespace realsense {

class RSColorNode /*: public avs::graph::Node*/ {
public:
    RSColorNode();
    ~RSColorNode();

    // Configure before start
    void setSerial(const std::string& s) { serial_ = s; }
    void setResolution(int w, int h, int fps=30) { cfg_.color_w=w; cfg_.color_h=h; cfg_.fps=fps; cfg_.enable_color=true; }

    bool start();
    void stop();

    std::optional<avs::adapters::realsense::RSFrame> fetch();

private:
    std::shared_ptr<avs::adapters::realsense::RealSenseDeviceManager> dev_;
    std::unique_ptr<avs::adapters::realsense::RealSenseFrameGrabber> grab_;
    avs::adapters::realsense::RSConfig cfg_;
    std::string serial_;
};

}}}

// Expected to be registered via AVS_REGISTER_NODE in .cpp
