#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <optional>

#include <avs/core/export.hpp>
#include <avs/plugin/plugin_api.hpp>

namespace avs::plugin {

struct LoadedPlugin {
    std::filesystem::path path;   // full path to the loaded .dll
    PluginInfo            info{}; // metadata
};

// Load all plugins from a directory. Returns successfully loaded plugins.
// The function is idempotent per path: repeated calls won't re-load the same file.
AVS_API std::vector<LoadedPlugin> load_all_from_dir(const std::filesystem::path& dir);

// Environment-aware bootstrap. Uses:
//   1) AVS_PLUGIN_DIR env var (semicolon/colon-separated)
//   2) Default: ./plugins next to the executable
AVS_API std::vector<LoadedPlugin> bootstrap_from_env_or_default(const std::filesystem::path& exe_dir);

// Return a list of all loaded plugin module paths.
AVS_API std::vector<std::filesystem::path> currently_loaded();

} // namespace avs::plugin
