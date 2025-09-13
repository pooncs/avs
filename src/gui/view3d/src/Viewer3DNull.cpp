#include "gui/view3d/Viewer3D.hpp"

namespace avs { namespace gui {

struct Viewer3D::Impl { };

Viewer3D::Viewer3D(const Settings&) : impl_(new Impl{}) {}
Viewer3D::~Viewer3D() = default;

void Viewer3D::setPointCloud(const std::vector<std::array<float,3>>&, const std::vector<std::array<float,3>>*) {}
void Viewer3D::setPoses(const std::vector<Pose>&, float) {}
void Viewer3D::setCameraLookAt(const std::array<float,3>&, const std::array<float,3>&, const std::array<float,3>&) {}
RenderImage Viewer3D::renderOnce() { return {}; }

}} // namespace avs::gui
