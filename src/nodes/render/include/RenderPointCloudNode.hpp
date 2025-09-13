#pragma once
#include <string>
#include <vector>
#include <array>
#include <memory>

#include "gui/view3d/Viewer3D.hpp"

namespace avs { namespace nodes { namespace render {

// A simple node that renders a PLY/PCD point cloud to a PNG using Open3D offscreen renderer.
class RenderPointCloudNode /* : public graph::Node */ {
public:
    struct Params {
        std::string input_pcd_path; // .ply/.pcd
        std::string output_png_path; // result image
        int width = 1280;
        int height = 720;
        float fov_deg = 60.f;
        std::array<float,3> lookat{0,0,0};
        std::array<float,3> eye{3,3,3};
        std::array<float,3> up{0,1,0};
        float point_size = 2.f;
    };

    RenderPointCloudNode() = default;
    explicit RenderPointCloudNode(const Params& p) : params_(p) {}

    bool run();

private:
    Params params_{};
};

}}} // namespace
