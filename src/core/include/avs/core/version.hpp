#pragma once
#include <cstdint>

namespace avs::core {
// Engine version (semantic)
static constexpr std::uint32_t VERSION_MAJOR = 0;
static constexpr std::uint32_t VERSION_MINOR = 1;
static constexpr std::uint32_t VERSION_PATCH = 0;

// Plugin ABI version. Bump on breaking changes to plugin C API.
static constexpr std::uint32_t PLUGIN_ABI_VERSION = 1;
}
