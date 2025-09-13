
#include "avs/nodes/inspection/BasicInspectionNodes.h"
#include <cmath>

namespace avs { namespace nodes { namespace inspection {

avs::Result MeshBoundsNode::Process(avs::graph::Context&){
    if(!scan_) return avs::Result::failure("MeshBoundsNode: missing input scan");
    aabb_ = scan_->ComputeAABB();
    if(!aabb_) return avs::Result::failure("AABB unavailable (no geometry or Open3D disabled)");
    return avs::Result::success();
}

avs::Result SurfaceAreaNode::Process(avs::graph::Context&){
    if(!scan_) return avs::Result::failure("SurfaceAreaNode: missing input scan");
    area_ = scan_->SurfaceArea();
    if(!area_) return avs::Result::failure("Surface area unavailable (not a mesh or Open3D disabled)");
    return avs::Result::success();
}

avs::Result PointDistanceNode::Process(avs::graph::Context&){
    const double dx = A[0]-B[0];
    const double dy = A[1]-B[1];
    const double dz = A[2]-B[2];
    dist_ = std::sqrt(dx*dx+dy*dy+dz*dz);
    return avs::Result::success();
}

}}}
