#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <memory>

#include <avs/core/version.hpp>
#include <avs/core/export.hpp>

namespace avs::plugin {

struct PluginInfo {
    std::uint32_t abi_version;  // must equal avs::core::PLUGIN_ABI_VERSION
    const char*   name;         // short name, e.g., "sample_nodes"
    const char*   version;      // semantic version string, e.g., "0.1.0"
    const char*   description;  // brief human-readable description
};

// --- Plugin entry points ---------------------------------------------------
// A plugin DLL **must** export the following two C symbols.

extern "C" {
    // Returns compile-time constant info about the plugin. No allocations.
    AVS_API PluginInfo avs_plugin_get_info();

    // Called after the DLL is loaded and validated. The plugin should
    // register its nodes by invoking the existing AVS_REGISTER_NODE(Class)
    // macro in this function body. Return 0 on success, non-zero on error.
    AVS_API int avs_plugin_register();
}

} // namespace avs::plugin
