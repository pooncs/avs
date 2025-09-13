#include "gui/view3d/Viewer3D.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#ifdef WITH_OPEN3D
// Open3D C++ API
#include <open3d/Open3D.h>
#include <open3d/visualization/rendering/Camera.h>
#include <open3d/visualization/rendering/filament/FilamentEngine.h>
#include <open3d/visualization/rendering/filament/FilamentRenderer.h>
#endif

namespace avs { namespace gui {

struct Viewer3D::Impl {
#ifdef WITH_OPEN3D
    using Application = open3d::visualization::gui::Application;
    using FilamentRenderer = open3d::visualization::rendering::FilamentRenderer;
    using EngineInstance = open3d::visualization::rendering::EngineInstance;
    using Open3DScene = open3d::visualization::rendering::Open3DScene;
    using Camera = open3d::visualization::rendering::Camera;
    using MaterialRecord = open3d::visualization::rendering::MaterialRecord;

    struct AppGuard {
        Application &app;
        AppGuard() : app(Application::GetInstance()) { int argc=0; const char** argv=nullptr; app.Initialize(argc, argv); }
        ~AppGuard() { app.OnTerminate(); }
    };

    AppGuard app_{}; // RAII init/terminate

    int width_ = 0, height_ = 0; float fov_deg_=60.f, near_=0.01f, far_=100.f; float bg_[4]{0,0,0,1};

    // Order matters: scene must be destroyed BEFORE renderer per Open3D guidance
    std::unique_ptr<Open3DScene> scene_;
    std::unique_ptr<FilamentRenderer> renderer_;

    std::shared_ptr<open3d::geometry::PointCloud> pcd_;
    std::vector<Pose> poses_;
    float axis_size_ = 0.1f;
    float point_size_ = 2.f;

    bool explicit_cam_ = false;
    std::array<float,3> cam_center_{0,0,0};
    std::array<float,3> cam_eye_{3,3,3};
    std::array<float,3> cam_up_{0,1,0};

    std::mutex mtx_;

    Impl(const Viewer3D::Settings& s) {
        width_ = s.width; height_ = s.height; fov_deg_ = s.fov_deg; near_ = s.near_clip; far_ = s.far_clip; point_size_ = s.point_size; for(int i=0;i<4;++i) bg_[i]=s.bg_rgba[i];
        renderer_.reset(new FilamentRenderer(EngineInstance::GetInstance(), width_, height_, EngineInstance::GetResourceManager()));
        scene_.reset(new Open3DScene(*renderer_));
        scene_->ShowAxes(s.show_axes);
        scene_->SetBackground(Eigen::Vector4f(bg_[0],bg_[1],bg_[2],bg_[3]));
        scene_->SetLighting(Open3DScene::LightingProfile::NO_SHADOWS, Eigen::Vector3f(0,0,0));
    }

    ~Impl() = default;

    static Eigen::Matrix4d PoseToMatrix(const Pose& p) {
        const double x=p.q[0], y=p.q[1], z=p.q[2], w=p.q[3];
        Eigen::Quaterniond q(w,x,y,z); // note: Eigen uses w,x,y,z
        Eigen::Matrix3d R = q.normalized().toRotationMatrix();
        Eigen::Matrix4d T = Eigen::Matrix4d::Identity();
        T.block<3,3>(0,0)=R; T(0,3)=p.t[0]; T(1,3)=p.t[1]; T(2,3)=p.t[2];
        return T;
    }

    void rebuildSceneGeometries() {
        std::lock_guard<std::mutex> lk(mtx_);
        scene_->ClearGeometry();
        if (pcd_) {
            MaterialRecord mat; mat.shader = "defaultUnlit"; mat.point_size = point_size_;
            scene_->AddGeometry("pcd", pcd_.get(), mat);
        }
        // Poses as coordinate frames
        int idx = 0;
        for (const auto& p : poses_) {
            auto frame = open3d::geometry::TriangleMesh::CreateCoordinateFrame(axis_size_);
            frame->Transform(PoseToMatrix(p));
            MaterialRecord mat; mat.shader = "defaultUnlit";
            scene_->AddGeometry("pose_"+std::to_string(idx++), frame.get(), mat);
        }

        // Auto camera if not explicit
        if (!explicit_cam_) {
            const auto& aabb = scene_->GetBoundingBox();
            Eigen::Vector3d center = aabb.GetCenter();
            double extent = aabb.GetMaxExtent(); if (extent <= 1e-6) extent = 1.0;
            Eigen::Vector3d eye = center + Eigen::Vector3d(0.8, 0.8, 0.8) * (extent * 2.0);
            scene_->GetCamera()->SetProjection(fov_deg_, float(width_)/float(height_), near_, far_, Camera::FovType::Vertical);
            scene_->GetCamera()->LookAt(center.cast<float>(), eye.cast<float>(), Eigen::Vector3f(0.f,1.f,0.f));
        } else {
            Eigen::Vector3f c(cam_center_[0],cam_center_[1],cam_center_[2]);
            Eigen::Vector3f e(cam_eye_[0],cam_eye_[1],cam_eye_[2]);
            Eigen::Vector3f u(cam_up_[0],cam_up_[1],cam_up_[2]);
            scene_->GetCamera()->SetProjection(fov_deg_, float(width_)/float(height_), near_, far_, Camera::FovType::Vertical);
            scene_->GetCamera()->LookAt(c,e,u);
        }
    }

    RenderImage renderOnce() {
        rebuildSceneGeometries();
        auto& app = Application::GetInstance();
        auto img = app.RenderToImage(*renderer_, scene_->GetView(), scene_->GetScene(), width_, height_);
        RenderImage out; out.width = width_; out.height = height_;
        if (img) {
            // Expect 4 channels uint8
            out.rgba.resize(width_*height_*4);
            const auto &tensor = img->AsTensor();
            if (tensor.GetDtype() == open3d::core::Dtype::UInt8) {
                std::memcpy(out.rgba.data(), tensor.GetDataPtr<uint8_t>(), out.rgba.size());
            } else {
                // Fallback: convert
                auto img8 = img->ConvertTo(open3d::geometry::Image::ColorToDepthConversionType::kNone);
                const auto &t8 = img8->AsTensor();
                out.rgba.assign(t8.GetDataPtr<uint8_t>(), t8.GetDataPtr<uint8_t>() + out.rgba.size());
            }
        }
        return out;
    }
#else
    // Null stub when Open3D is not available
    int width_=0, height_=0; float bg_[4]{0,0,0,1};
    RenderImage renderOnce() { return {}; }
    void setPCD(...) {}
#endif
};

Viewer3D::Viewer3D(const Settings& s) : impl_(new Impl(s)) {}
Viewer3D::~Viewer3D() = default;

void Viewer3D::setPointCloud(const std::vector<std::array<float,3>>& points,
                             const std::vector<std::array<float,3>>* colors) {
#ifdef WITH_OPEN3D
    auto pcd = std::make_shared<open3d::geometry::PointCloud>();
    pcd->points_.reserve(points.size());
    for (const auto& p : points) pcd->points_.emplace_back(double(p[0]), double(p[1]), double(p[2]));
    if (colors && colors->size()==points.size()) {
        pcd->colors_.reserve(points.size());
        for (const auto& c : *colors) pcd->colors_.emplace_back(double(c[0]), double(c[1]), double(c[2]));
    }
    impl_->pcd_ = std::move(pcd);
#else
    (void)points; (void)colors;
#endif
}

void Viewer3D::setPoses(const std::vector<Pose>& poses, float axis_size) {
#ifdef WITH_OPEN3D
    impl_->poses_ = poses; impl_->axis_size_ = axis_size;
#else
    (void)poses; (void)axis_size;
#endif
}

void Viewer3D::setCameraLookAt(const std::array<float,3>& center,
                               const std::array<float,3>& eye,
                               const std::array<float,3>& up) {
#ifdef WITH_OPEN3D
    impl_->explicit_cam_ = true; impl_->cam_center_ = center; impl_->cam_eye_ = eye; impl_->cam_up_ = up;
#else
    (void)center; (void)eye; (void)up;
#endif
}

RenderImage Viewer3D::renderOnce() { return impl_->renderOnce(); }

}} // namespace avs::gui
