#include "nodes/render/RenderPointCloudNode.hpp"

#include <fstream>

#ifdef WITH_OPEN3D
#include <open3d/Open3D.h>
#endif

// #include "graph/Registry.hpp" // AVS_REGISTER_NODE

namespace avs { namespace nodes { namespace render {

bool RenderPointCloudNode::run() {
#ifdef WITH_OPEN3D
    using namespace open3d;
    // Load point cloud
    std::shared_ptr<geometry::PointCloud> pcd = io::CreatePointCloudFromFile(params_.input_pcd_path);
    if (!pcd || pcd->points_.empty()) return false;

    gui::Viewer3D::Settings s; s.width=params_.width; s.height=params_.height; s.fov_deg=params_.fov_deg; s.point_size=params_.point_size;
    gui::Viewer3D viewer(s);

    // Convert pcd to vectors
    std::vector<std::array<float,3>> pts; pts.reserve(pcd->points_.size());
    std::vector<std::array<float,3>> cols; cols.reserve(pcd->colors_.size());
    for (const auto& p : pcd->points_) pts.push_back({float(p.x()), float(p.y()), float(p.z())});
    if (!pcd->colors_.empty()) {
        for (const auto& c : pcd->colors_) cols.push_back({float(c.x()), float(c.y()), float(c.z())});
        viewer.setPointCloud(pts, &cols);
    } else {
        viewer.setPointCloud(pts, nullptr);
    }

    viewer.setCameraLookAt(params_.lookat, params_.eye, params_.up);
    auto img = viewer.renderOnce();

    if (img.width == 0 || img.height == 0 || img.rgba.empty()) return false;

    // Save PNG via Open3D image writer
    geometry::Image oimg; oimg.Prepare(img.width, img.height, 4, 1);
    std::copy(img.rgba.begin(), img.rgba.end(), oimg.data_.begin());
    return io::WriteImage(params_.output_png_path, oimg);
#else
    (void)params_;
    return false;
#endif
}

}}} // namespace

// Register into AVS graph registry (guarded)
#ifdef WITH_OPEN3D
// AVS_REGISTER_NODE(avs::nodes::render::RenderPointCloudNode);
#endif
