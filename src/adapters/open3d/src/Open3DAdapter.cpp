#include "avs/adapters/open3d/Open3DAdapter.hpp"

#if !defined(WITH_OPEN3D)
#  error "This translation unit requires WITH_OPEN3D"
#endif

#include <open3d/Open3D.h>
#include <stdexcept>
#include <mutex>

namespace avs { namespace adapters { namespace open3d {

struct PointCloud::Impl {
    std::shared_ptr<::open3d::geometry::PointCloud> pc;
    Impl() : pc(std::make_shared<::open3d::geometry::PointCloud>()) {}
};

static inline PointCloud::Transform4x4 ToArray(const Eigen::Matrix4d& m) {
    PointCloud::Transform4x4 a{};
    for (int r=0; r<4; ++r) for (int c=0; c<4; ++c) a[r*4+c] = m(r,c);
    return a;
}

std::shared_ptr<PointCloud> PointCloud::Create() {
    auto p = std::shared_ptr<PointCloud>(new PointCloud());
    p->impl_ = std::make_shared<Impl>();
    return p;
}

std::shared_ptr<PointCloud> PointCloud::FromXYZ(const std::vector<std::array<double,3>>& xyz) {
    auto p = Create();
    p->impl_->pc->points_.reserve(xyz.size());
    for (auto &v : xyz) p->impl_->pc->points_.emplace_back(v[0], v[1], v[2]);
    return p;
}

std::shared_ptr<PointCloud> PointCloud::FromXYZRGB(const std::vector<std::array<double,3>>& xyz,
                                                   const std::vector<std::array<uint8_t,3>>& rgb) {
    auto p = Create();
    p->impl_->pc->points_.reserve(xyz.size());
    p->impl_->pc->colors_.reserve(xyz.size());
    const size_t n = xyz.size();
    for (size_t i=0; i<n; ++i) {
        const auto &v = xyz[i];
        p->impl_->pc->points_.emplace_back(v[0], v[1], v[2]);
        if (i < rgb.size()) {
            const auto &c = rgb[i];
            p->impl_->pc->colors_.emplace_back(c[0]/255.0, c[1]/255.0, c[2]/255.0);
        }
    }
    return p;
}

std::size_t PointCloud::size() const noexcept { return impl_->pc->points_.size(); }

bool PointCloud::hasNormals() const noexcept { return !impl_->pc->normals_.empty(); }

std::shared_ptr<PointCloud> PointCloud::voxelDownsample(double voxel) const {
    auto ds = impl_->pc->VoxelDownSample(voxel);
    auto p = Create();
    p->impl_->pc = ds;
    return p;
}

void PointCloud::estimateNormals(double radius, int max_nn) {
    ::open3d::geometry::KDTreeSearchParamHybrid param(radius, max_nn);
    impl_->pc->EstimateNormals(param);
}

PointCloud::ICPResult PointCloud::icpTo(const PointCloud& target,
                                        double max_corr_dist,
                                        int max_iterations) const {
    using namespace ::open3d::pipelines::registration;
    ICPResult r;
    Eigen::Matrix4d init = Eigen::Matrix4d::Identity();
    auto criteria = ICPConvergenceCriteria();
    criteria.max_iteration_ = max_iterations;
    auto result = RegistrationICP(*impl_->pc, *target.impl_->pc, max_corr_dist, init,
                                  TransformationEstimationPointToPlane(), criteria);
    r.transform = ToArray(result.transformation_);
    r.fitness = result.fitness_;
    r.inlier_rmse = result.inlier_rmse_;

    auto aligned = std::make_shared<::open3d::geometry::PointCloud>(*impl_->pc);
    aligned->Transform(result.transformation_);
    r.aligned = Create();
    r.aligned->impl_->pc = aligned;
    return r;
}

PointCloud::PlaneSegResult PointCloud::segmentPlane(double distance_threshold,
                                                    int ransac_n,
                                                    int num_iterations) const {
    PlaneSegResult out;
    auto res = impl_->pc->SegmentPlane(distance_threshold, ransac_n, num_iterations);
    const auto &coeffs = std::get<0>(res);
    out.model.coeffs = {coeffs[0], coeffs[1], coeffs[2], coeffs[3]};

    auto inliers_idx = std::get<1>(res);
    auto inliers = impl_->pc->SelectByIndex(inliers_idx);

    // outliers are the rest
    std::vector<size_t> complement; complement.reserve(impl_->pc->points_.size() - inliers_idx.size());
    {
        std::vector<char> mark(impl_->pc->points_.size(), 0);
        for (auto i : inliers_idx) if (i < mark.size()) mark[i] = 1;
        for (size_t i=0;i<mark.size();++i) if (!mark[i]) complement.push_back(i);
    }
    auto outliers = impl_->pc->SelectByIndex(complement);

    out.inliers = Create(); out.inliers->impl_->pc = inliers;
    out.outliers = Create(); out.outliers->impl_->pc = outliers;
    return out;
}

bool PointCloud::savePCD(const char* path) const {
    return ::open3d::io::WritePointCloud(std::string(path), *impl_->pc);
}

std::shared_ptr<PointCloud> PointCloud::loadPCD(const char* path) {
    auto p = Create();
    ::open3d::io::ReadPointCloud(std::string(path), *p->impl_->pc);
    return p;
}

PointCloud::~PointCloud() = default;
PointCloud::PointCloud() = default;
PointCloud::PointCloud(const PointCloud&) = default;
PointCloud::PointCloud(PointCloud&&) noexcept = default;
PointCloud& PointCloud::operator=(const PointCloud&) = default;
PointCloud& PointCloud::operator=(PointCloud&&) noexcept = default;

}}} // namespace
