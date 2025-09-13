#pragma once
#include "avs/nodes/pointcloud/PointCloudNodeCommon.hpp"
#include "avs/adapters/open3d/Open3DAdapter.hpp"
#include <memory>

namespace avs { namespace nodes { namespace pointcloud {

class PlaneSegmentationNode {
public:
    using Cloud = avs::adapters::open3d::PointCloud;

    struct Output {
        Cloud::PlaneSegResult seg;
    };

    Output process(const std::shared_ptr<Cloud>& input,
                   const PlaneSegParams& p) const {
        Output o; if (!input) return o; o.seg = input->segmentPlane(p.distance_threshold, p.ransac_n, p.num_iterations); return o;
    }

    static const char* Name() { return "PlaneSegmentation"; }
    static const char* Category() { return kCategory; }
};

}}} // namespace

AVS_REGISTER_NODE(avs::nodes::pointcloud::PlaneSegmentationNode)
