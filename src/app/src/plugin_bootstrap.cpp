#include <filesystem>
#include <vector>
#include <avs/plugin/plugin_loader.hpp>

namespace avs_app {

// Call this once during application startup after logging and registry are ready.
void load_plugins_at_startup(const std::filesystem::path& exe_dir) {
    (void)avs::plugin::bootstrap_from_env_or_default(exe_dir);
}

} // namespace avs_app
