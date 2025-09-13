
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <chrono>

namespace avs {

// Lightweight Result pattern per context.txt
struct Result {
    bool ok{true};
    std::string msg;
    static Result success() { return {true, {}}; }
    static Result failure(std::string m) { return {false, std::move(m)}; }
};

namespace geometry {
    struct AABB { double min[3]{}, max[3]{}; };
}

namespace adapters { namespace creaform {

// Forward declarations to avoid exposing heavy deps in public header
class ScanImpl;

/// CreaformScan is a light RAII handle to an imported mesh/point cloud
class CreaformScan {
public:
    using time_point = std::chrono::system_clock::time_point;

    CreaformScan();
    ~CreaformScan();
    CreaformScan(CreaformScan&&) noexcept;
    CreaformScan& operator=(CreaformScan&&) noexcept;
    CreaformScan(const CreaformScan&) = delete;
    CreaformScan& operator=(const CreaformScan&) = delete;

    /// Load mesh/point-cloud from file. Supports .ply/.pcd/.obj/.stl when built WITH_OPEN3D.
    static std::pair<std::unique_ptr<CreaformScan>, Result> LoadFromFile(const std::string& path);

    /// Serialize basic metadata as JSON string (no dep on nlohmann/json in public headers)
    std::string MetadataJson() const;

    /// Returns axis-aligned bounding box if available
    std::optional<avs::geometry::AABB> ComputeAABB() const noexcept;

    /// Surface area (for triangle mesh). std::nullopt if not a mesh or unsupported
    std::optional<double> SurfaceArea() const noexcept;

    /// Point count (cloud) or vertex count (mesh)
    std::size_t VertexCount() const noexcept;

    /// Face/triangle count if mesh; 0 otherwise
    std::size_t FaceCount() const noexcept;

    /// True if the held data is a triangle mesh
    bool IsTriangleMesh() const noexcept;

    /// True if the held data is a point cloud
    bool IsPointCloud() const noexcept;

    /// Internal pointer opaque access (use carefully inside nodes built in the same module)
    const ScanImpl* Impl() const noexcept { return impl_.get(); }

private:
    std::unique_ptr<ScanImpl> impl_;
};

}} // namespace adapters::creaform
} // namespace avs
