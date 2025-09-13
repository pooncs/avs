
#include "nodes/photoneo/PhoxiGrabNode.h"
#include "core/logging.h"

namespace avs { namespace nodes { namespace photoneo {

core::Result PhoxiGrabNode::process(graph::ExecutionContext& ctx) {
    (void)ctx; // unused placeholder

    using namespace std::chrono;
    if (!dev_.isOpen()) {
        auto r = dev_.open(serial_.empty() ? std::string("InstalledExamples-PhoXi-example") : serial_, milliseconds(connect_timeout_ms_));
        if (!r.ok) {
            CORE_LOG_ERROR("PhoxiGrabNode: {}", r.msg);
            return {false, r.msg};
        }
        r = dev_.startAcquisition();
        if (!r.ok) {
            CORE_LOG_ERROR("PhoxiGrabNode: {}", r.msg);
            return {false, r.msg};
        }
    }

    auto r = dev_.grabPointCloud(cloud_, milliseconds(frame_timeout_ms_), organized_);
    if (!r.ok) {
        CORE_LOG_ERROR("PhoxiGrabNode: {}", r.msg);
        return {false, r.msg};
    }

    CORE_LOG_INFO("PhoxiGrabNode: grabbed {} points{}", cloud_.points.size(), organized_ ? " (organized)" : "");

    // TODO: publish cloud_ to outputs once point cloud type is standardized in AVS
    return {true, {}};
}

}}} // namespace
