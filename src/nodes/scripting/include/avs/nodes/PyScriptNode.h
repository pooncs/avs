#pragma once

#include <string>
#include <unordered_map>
#include <memory>

// Only build this node when Python is enabled
#if defined(WITH_PYTHON)

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "avs/scripting/PyEngine.h"

// We try to include AVS graph headers if available; otherwise we compile a disabled stub.
#if __has_include(<avs/graph/Node.h>) && __has_include(<avs/graph/Registry.h>) && __has_include(<avs/core/Result.h>)
  #define AVS_HAS_GRAPH_API 1
  #include <avs/graph/Node.h>
  #include <avs/graph/Registry.h>
  #include <avs/core/Result.h>
#else
  #define AVS_HAS_GRAPH_API 0
#endif

namespace avs { namespace nodes {

#if AVS_HAS_GRAPH_API

/**
 * PyScriptNode: executes a Python function `process(inputs: dict, params: dict) -> dict` from a file.
 * Parameters:
 *  - script_path (string)
 *  - entry (string, default: "process")
 * Ports:
 *  - Dynamically read inputs/outputs via Node API (expects JSON/primitive types)
 */
class PyScriptNode final : public graph::Node {
public:
    PyScriptNode();
    ~PyScriptNode() override = default;

    // Optional: expose parameters via reflection (if the base Node supports it).
    // Here we provide simple accessors so the app/inspector can hook in.
    void setScriptPath(std::string path) { script_path_ = std::move(path); }
    const std::string& scriptPath() const noexcept { return script_path_; }
    void setEntry(std::string e) { entry_ = std::move(e); }
    const std::string& entry() const noexcept { return entry_; }

protected:
    // NOTE: We assume the base class uses `Result process()` (common pattern in this repo).
    avs::Result process() override;

private:
    std::string script_path_;
    std::string entry_ {"process"};
    scripting::PyEngine engine_;
};

#else // !AVS_HAS_GRAPH_API

// If graph API isn't available at build time, provide a placeholder so the target still compiles
class PyScriptNode final {
public:
    PyScriptNode() = default;
};

#endif // AVS_HAS_GRAPH_API

}} // namespace avs::nodes

#endif // WITH_PYTHON
