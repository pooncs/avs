
#pragma once
#include <string>
#include <memory>
#include <optional>
#include "avs/adapters/creaform/CreaformAdapter.h"

// Try to include AVS graph API; define minimal shims if missing
#if __has_include(<avs/graph/Node.h>)
  #include <avs/graph/Node.h>
  #include <avs/graph/Registry.h>
#else
  namespace avs { namespace graph {
    struct Context{};
    struct PortInfo{ const char* name; };
    class Node{ public: virtual ~Node()=default; virtual const char* Title() const noexcept = 0; virtual avs::Result Process(Context&) = 0; };
  }}
  #ifndef AVS_REGISTER_NODE
    #define AVS_REGISTER_NODE(T)
  #endif
#endif

namespace avs { namespace nodes { namespace inspection {

/// Node: Import a Creaform scan file and output a CreaformScan handle
class CreaformImportNode : public avs::graph::Node {
public:
    CreaformImportNode() = default;
    const char* Title() const noexcept override { return "Creaform Import"; }

    // Simple properties
    void SetPath(std::string p) { path_ = std::move(p); }
    const std::string& GetPath() const { return path_; }

    std::shared_ptr<avs::adapters::creaform::CreaformScan> Output() const { return scan_; }

    avs::Result Process(avs::graph::Context&) override;

private:
    std::string path_;
    std::shared_ptr<avs::adapters::creaform::CreaformScan> scan_;
};

}}}

// Ensure registration macro expands when available
AVS_REGISTER_NODE(avs::nodes::inspection::CreaformImportNode)
