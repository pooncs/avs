#pragma once
#include <memory>
#include <vector>
#include <array>
#include <cstdint>

namespace avs { namespace adapters { namespace open3d {

// Minimal, Open3D-free public API (pImpl hides Open3D types)
class PointCloud {
public:
    struct PlaneModel { std::array<double,4> coeffs{}; };
    struct PlaneSegResult {
        PlaneModel model;
        std::shared_ptr<PointCloud> inliers;
        std::shared_ptr<PointCloud> outliers;
    };

    using Transform4x4 = std::array<double, 16>; // row-major

    static std::shared_ptr<PointCloud> Create();
    static std::shared_ptr<PointCloud> FromXYZ(const std::vector<std::array<double,3>>& xyz);
    static std::shared_ptr<PointCloud> FromXYZRGB(
        const std::vector<std::array<double,3>>& xyz,
        const std::vector<std::array<uint8_t,3>>& rgb);

    // Accessors
    std::size_t size() const noexcept;
    bool hasNormals() const noexcept;

    // Algorithms (non-destructive unless noted)
    std::shared_ptr<PointCloud> voxelDownsample(double voxel) const;
    void estimateNormals(double radius, int max_nn);

    struct ICPResult {
        std::shared_ptr<PointCloud> aligned;
        Transform4x4 transform{}; // source->target
        double fitness{0.0};
        double inlier_rmse{0.0};
    };
    ICPResult icpTo(const PointCloud& target,
                    double max_corr_dist,
                    int max_iterations) const;

    PlaneSegResult segmentPlane(double distance_threshold,
                                int ransac_n,
                                int num_iterations) const;

    // Serialization
    bool savePCD(const char* path) const; // ASCII/auto
    static std::shared_ptr<PointCloud> loadPCD(const char* path);

    // pImpl
    ~PointCloud();
    PointCloud(const PointCloud&);
    PointCloud(PointCloud&&) noexcept;
    PointCloud& operator=(const PointCloud&);
    PointCloud& operator=(PointCloud&&) noexcept;
private:
    PointCloud();
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

}}} // namespace avs::adapters::open3d
