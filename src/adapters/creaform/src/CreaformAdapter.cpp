
#include "avs/adapters/creaform/CreaformAdapter.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <mutex>

#ifdef WITH_OPEN3D
#include <open3d/geometry/TriangleMesh.h>
#include <open3d/geometry/PointCloud.h>
#include <open3d/io/PointCloudIO.h>
#include <open3d/io/TriangleMeshIO.h>
#endif

namespace avs { namespace adapters { namespace creaform {

struct CreaformScan::ScanImpl {
#ifdef WITH_OPEN3D
    std::shared_ptr<open3d::geometry::Geometry3D> geom; // Either Mesh or PointCloud
#endif
    std::string source_path;
    std::string scanner_serial;
    std::string scanner_model;
    time_point acquired_at{};
};

static bool has_ext(const std::string& path, const std::vector<std::string>& exts){
    auto pos = path.find_last_of('.');
    if(pos==std::string::npos) return false;
    std::string e = path.substr(pos+1);
    std::transform(e.begin(), e.end(), e.begin(), ::tolower);
    for(const auto& x: exts) if(e==x) return true; return false;
}

CreaformScan::CreaformScan() : impl_(std::make_unique<ScanImpl>()) {}
CreaformScan::~CreaformScan() = default;
CreaformScan::CreaformScan(CreaformScan&&) noexcept = default;
CreaformScan& CreaformScan::operator=(CreaformScan&&) noexcept = default;

std::pair<std::unique_ptr<CreaformScan>, Result>
CreaformScan::LoadFromFile(const std::string& path){
    auto scan = std::make_unique<CreaformScan>();
    scan->impl_->source_path = path;
    scan->impl_->acquired_at = std::chrono::system_clock::now();

#ifdef WITH_OPEN3D
    try {
        if(has_ext(path, {"ply","pcd","xyz","xyzn","xyzrgb"})){
            auto pcd = std::make_shared<open3d::geometry::PointCloud>();
            if(!open3d::io::ReadPointCloud(path, *pcd)){
                return {nullptr, Result::failure("Failed to read point cloud: "+path)};
            }
            scan->impl_->geom = pcd;
        } else if(has_ext(path, {"stl","obj","off","gltf","glb","fbx","ply"})){
            auto mesh = std::make_shared<open3d::geometry::TriangleMesh>();
            if(!open3d::io::ReadTriangleMesh(path, *mesh)){
                return {nullptr, Result::failure("Failed to read triangle mesh: "+path)};
            }
            scan->impl_->geom = mesh;
        } else {
            return {nullptr, Result::failure("Unsupported file extension: "+path)};
        }
    } catch(const std::exception& e){
        return {nullptr, Result::failure(std::string("Exception reading file: ")+e.what())};
    }
#else
    // Without Open3D, we cannot parse formats reliably. Only accept .ply ASCII with minimal validation.
    if(!has_ext(path, {"ply"})){
        return {nullptr, Result::failure("Open3D disabled: only .ply supported (not parsed). Enable WITH_OPEN3D.")};
    }
    // We keep metadata only; geometry ops will be limited.
#endif

    return { std::move(scan), Result::success() };
}

std::string CreaformScan::MetadataJson() const {
    std::ostringstream oss;
    auto t = std::chrono::system_clock::to_time_t(impl_->acquired_at);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    oss << "{
";
    oss << "  "source_path": "" << impl_->source_path << "",
";
    oss << "  "acquired_at": "" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << ""
";
    oss << "}";
    return oss.str();
}

std::optional<avs::geometry::AABB> CreaformScan::ComputeAABB() const noexcept {
#ifdef WITH_OPEN3D
    if(!impl_->geom) return std::nullopt;
    if(auto mesh = std::dynamic_pointer_cast<open3d::geometry::TriangleMesh>(impl_->geom)){
        auto aabb = mesh->GetAxisAlignedBoundingBox();
        avs::geometry::AABB out{};
        for(int i=0;i<3;++i){ out.min[i]=aabb.min_bound_(i); out.max[i]=aabb.max_bound_(i);} 
        return out;
    } else if(auto pcd = std::dynamic_pointer_cast<open3d::geometry::PointCloud>(impl_->geom)){
        auto aabb = pcd->GetAxisAlignedBoundingBox();
        avs::geometry::AABB out{};
        for(int i=0;i<3;++i){ out.min[i]=aabb.min_bound_(i); out.max[i]=aabb.max_bound_(i);} 
        return out;
    }
#endif
    return std::nullopt;
}

std::optional<double> CreaformScan::SurfaceArea() const noexcept {
#ifdef WITH_OPEN3D
    if(!impl_->geom) return std::nullopt;
    if(auto mesh = std::dynamic_pointer_cast<open3d::geometry::TriangleMesh>(impl_->geom)){
        return mesh->GetSurfaceArea();
    }
#endif
    return std::nullopt;
}

std::size_t CreaformScan::VertexCount() const noexcept {
#ifdef WITH_OPEN3D
    if(!impl_->geom) return 0;
    if(auto mesh = std::dynamic_pointer_cast<open3d::geometry::TriangleMesh>(impl_->geom)){
        return mesh->vertices_.size();
    } else if(auto pcd = std::dynamic_pointer_cast<open3d::geometry::PointCloud>(impl_->geom)){
        return pcd->points_.size();
    }
#endif
    return 0;
}

std::size_t CreaformScan::FaceCount() const noexcept {
#ifdef WITH_OPEN3D
    if(!impl_->geom) return 0;
    if(auto mesh = std::dynamic_pointer_cast<open3d::geometry::TriangleMesh>(impl_->geom)){
        return mesh->triangles_.size();
    }
#endif
    return 0;
}

bool CreaformScan::IsTriangleMesh() const noexcept {
#ifdef WITH_OPEN3D
    if(!impl_->geom) return false;
    return (bool)std::dynamic_pointer_cast<open3d::geometry::TriangleMesh>(impl_->geom);
#else
    return false;
#endif
}

bool CreaformScan::IsPointCloud() const noexcept {
#ifdef WITH_OPEN3D
    if(!impl_->geom) return false;
    return (bool)std::dynamic_pointer_cast<open3d::geometry::PointCloud>(impl_->geom);
#else
    return false;
#endif
}

}}} // namespace
