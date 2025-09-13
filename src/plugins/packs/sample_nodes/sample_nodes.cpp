#include <string>
#include <avs/core/version.hpp>
#include <avs/plugin/plugin_api.hpp>
#include <avs/graph/node.hpp>      // Provided by the repo
#include <avs/graph/registry.hpp>  // Provides AVS_REGISTER_NODE

// Minimal example node that simply forwards its input to output.
namespace avs::nodes {

class SamplePassThrough final : public avs::graph::Node {
public:
    SamplePassThrough() = default;
    ~SamplePassThrough() override = default;

    // Pseudo-overrides: Adjust to actual Node API available in the repo.
    // Here we provide minimal placeholders to keep it compilable once the
    // real Node interface exists.
    const char* type() const noexcept override { return "sample.PassThrough"; }
};

} // namespace avs::nodes

extern "C" {

AVS_API avs::plugin::PluginInfo avs_plugin_get_info() {
    static const avs::plugin::PluginInfo k = {
        avs::core::PLUGIN_ABI_VERSION,
        "sample_nodes",
        "0.1.0",
        "Sample node pack demonstrating AVS plugin registration"
    };
    return k;
}

AVS_API int avs_plugin_register() {
    // Register nodes in this pack. Adjust categories/types to your taxonomy.
    AVS_REGISTER_NODE(avs::nodes::SamplePassThrough);
    return 0; // success
}

} // extern "C"
