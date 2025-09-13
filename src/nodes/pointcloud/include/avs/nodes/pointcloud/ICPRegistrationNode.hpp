#pragma once
#include "avs/nodes/pointcloud/PointCloudNodeCommon.hpp"
#include "avs/adapters/open3d/Open3DAdapter.hpp"
#include <memory>

namespace avs { namespace nodes { namespace pointcloud {

class ICPRegistrationNode {
public:
    using Cloud = avs::adapters::open3d::PointCloud;

    struct Output {
        std::shared_ptr<Cloud> aligned;
        Cloud::Transform4x4 transform{};
        double fitness{0.0};
        double inlier_rmse{0.0};
    };

    Output process(const std::shared_ptr<Cloud>& source,
                   const std::shared_ptr<Cloud>& target,
                   const ICPParams& p) const {
        Output o; if (!source || !target) return o;
        auto r = source->icpTo(*target, p.max_corr_dist, p.max_iterations);
        o.aligned = r.aligned; o.transform = r.transform; o.fitness = r.fitness; o.inlier_rmse = r.inlier_rmse; return o;
    }

    static const char* Name() { return "ICPRegistration"; }
    static const char* Category() { return kCategory; }
};

}}} // namespace

AVS_REGISTER_NODE(avs::nodes::pointcloud::ICPRegistrationNode)
