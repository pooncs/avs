#pragma once
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace avs { namespace gui {

struct Pose {
    std::array<float,3> t{0,0,0}; // translation (m)
    std::array<float,4> q{0,0,0,1}; // quaternion (x,y,z,w)
};

struct RenderImage {
    int width = 0;
    int height = 0;
    std::vector<uint8_t> rgba; // width*height*4
};

class Viewer3D {
public:
    struct Settings {
        int width = 1280;
        int height = 720;
        float bg_rgba[4] {0.05f, 0.05f, 0.06f, 1.0f};
        float fov_deg = 60.0f; // vertical
        float near_clip = 0.01f;
        float far_clip = 100.0f;
        float point_size = 2.0f;
        bool show_axes = true;
    };

    explicit Viewer3D(const Settings& s);
    ~Viewer3D();

    // Replace the current point cloud (copy)
    void setPointCloud(const std::vector<std::array<float,3>>& points,
                       const std::vector<std::array<float,3>>* colors = nullptr);

    // Replace poses. axis_size in meters
    void setPoses(const std::vector<Pose>& poses, float axis_size = 0.1f);

    // Optional: set camera explicitly (world coordinates)
    void setCameraLookAt(const std::array<float,3>& center,
                         const std::array<float,3>& eye,
                         const std::array<float,3>& up = {0.f,1.f,0.f});

    // Render once and return RGBA buffer
    RenderImage renderOnce();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_; // pImpl ensures ABI stability & faster builds
};

}} // namespace avs::gui
