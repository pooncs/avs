#pragma once
#include <string>

// Avoid hard dependency on graph headers; define a lenient macro shim.
#ifndef AVS_REGISTER_NODE
#define AVS_REGISTER_NODE(...)
#endif

namespace avs { namespace nodes { namespace pointcloud {
inline constexpr const char* kCategory = "PointCloud";

struct VoxelDownsampleParams {
    double voxel_size{0.01};
};

struct ICPParams {
    double max_corr_dist{0.02};
    int max_iterations{50};
};

struct PlaneSegParams {
    double distance_threshold{0.01};
    int ransac_n{3};
    int num_iterations{1000};
};

}}} // namespace
