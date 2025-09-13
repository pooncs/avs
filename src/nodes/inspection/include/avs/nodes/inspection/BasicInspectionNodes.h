
#pragma once
#include <memory>
#include <optional>
#include <array>
#include "avs/adapters/creaform/CreaformAdapter.h"

#if __has_include(<avs/graph/Node.h>)
  #include <avs/graph/Node.h>
  #include <avs/graph/Registry.h>
#else
  namespace avs { namespace graph { struct Context{}; class Node{ public: virtual ~Node()=default; virtual const char* Title() const noexcept = 0; virtual avs::Result Process(Context&) = 0; }; }}
  #ifndef AVS_REGISTER_NODE
    #define AVS_REGISTER_NODE(T)
  #endif
#endif

namespace avs { namespace nodes { namespace inspection {

/// Node: Compute axis-aligned bounding box for a scan
class MeshBoundsNode : public avs::graph::Node {
public:
    MeshBoundsNode() = default;
    const char* Title() const noexcept override { return "Mesh/Cloud AABB"; }

    void SetInput(std::shared_ptr<avs::adapters::creaform::CreaformScan> scan) { scan_ = std::move(scan); }
    std::optional<avs::geometry::AABB> Output() const { return aabb_; }

    avs::Result Process(avs::graph::Context&) override;
private:
    std::shared_ptr<avs::adapters::creaform::CreaformScan> scan_;
    std::optional<avs::geometry::AABB> aabb_;
};

/// Node: Compute surface area if triangle mesh
class SurfaceAreaNode : public avs::graph::Node {
public:
    SurfaceAreaNode() = default;
    const char* Title() const noexcept override { return "Surface Area"; }

    void SetInput(std::shared_ptr<avs::adapters::creaform::CreaformScan> scan) { scan_ = std::move(scan); }
    std::optional<double> Output() const { return area_; }

    avs::Result Process(avs::graph::Context&) override;
private:
    std::shared_ptr<avs::adapters::creaform::CreaformScan> scan_;
    std::optional<double> area_;
};

/// Node: Compute Euclidean distance between two 3D points
class PointDistanceNode : public avs::graph::Node {
public:
    PointDistanceNode() = default;
    const char* Title() const noexcept override { return "Point Distance"; }

    void SetPoints(const std::array<double,3>& a, const std::array<double,3>& b){ A=a; B=b; }
    double Output() const { return dist_; }

    avs::Result Process(avs::graph::Context&) override;
private:
    std::array<double,3> A{0,0,0}, B{0,0,0};
    double dist_{0};
};

}}}

AVS_REGISTER_NODE(avs::nodes::inspection::MeshBoundsNode)
AVS_REGISTER_NODE(avs::nodes::inspection::SurfaceAreaNode)
AVS_REGISTER_NODE(avs::nodes::inspection::PointDistanceNode)
