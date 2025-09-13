#pragma once
#include "avs/nodes/pointcloud/PointCloudNodeCommon.hpp"
#include "avs/adapters/open3d/Open3DAdapter.hpp"
#include <memory>

namespace avs { namespace nodes { namespace pointcloud {

class VoxelDownsampleNode {
public:
    using Cloud = avs::adapters::open3d::PointCloud;
    VoxelDownsampleNode() = default;

    std::shared_ptr<Cloud> process(const std::shared_ptr<Cloud>& input,
                                   const VoxelDownsampleParams& p) const {
        return input ? input->voxelDownsample(p.voxel_size) : nullptr;
    }

    static const char* Name() { return "VoxelDownsample"; }
    static const char* Category() { return kCategory; }
};

}}} // namespace

AVS_REGISTER_NODE(avs::nodes::pointcloud::VoxelDownsampleNode)
