
#pragma once

#include <string>
#include <chrono>
#include "adapters/photoneo/PhoxiDevice.h"

// Minimal dependency on AVS graph. We assume these headers exist in the repo.
#include "graph/Node.h"
#include "graph/Registry.h"

namespace avs { namespace nodes { namespace photoneo {

class PhoxiGrabNode final : public graph::Node {
public:
    PhoxiGrabNode() = default;
    ~PhoxiGrabNode() override = default;

    // Configure before first run
    void setSerial(const std::string& s) { serial_ = s; }
    void setOrganized(bool v) { organized_ = v; }
    void setConnectTimeoutMs(int ms) { connect_timeout_ms_ = ms; }
    void setFrameTimeoutMs(int ms) { frame_timeout_ms_ = ms; }

    // Expose last captured cloud for downstream consumers that access node state
    const adapters::photoneo::PointCloud& cloud() const { return cloud_; }

    // --- graph::Node API ---
    core::Result process(graph::ExecutionContext& ctx) override;

private:
    adapters::photoneo::PhoxiDevice dev_{};
    adapters::photoneo::PointCloud cloud_{};
    std::string serial_{}; // e.g. "InstalledExamples-PhoXi-example" or HW ID
    bool organized_{true};
    int connect_timeout_ms_{5000};
    int frame_timeout_ms_{10000};
};

}}} // namespace avs::nodes::photoneo

// Register into node registry
AVS_REGISTER_NODE(avs::nodes::photoneo::PhoxiGrabNode, "Sensors/Photoneo/PhoXiGrab");
