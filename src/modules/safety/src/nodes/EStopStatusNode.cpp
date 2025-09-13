
// Node registers with graph if headers are available.
#if __has_include(<avs/graph/node.hpp>) && __has_include(<avs/graph/registry.hpp>)
  #include <string>
  #include <vector>
  #include <utility>
  #include <cstdint>
  #include <avs/graph/node.hpp>
  #include <avs/graph/registry.hpp>
  #include "avs/safety/SafetyContext.hpp"
  #include "avs/safety/SafetyState.hpp"

  namespace avs { namespace nodes {

  class EStopStatusNode final : public avs::graph::Node {
  public:
    EStopStatusNode() = default;
    ~EStopStatusNode() override = default;

    const char* display_name() const noexcept override { return "E-Stop Status"; }

    // Minimalistic I/O contract: no inputs, two outputs: ok_to_run (bool), state (string)
    void process() override {
      auto& mgr = avs::safety::SafetyContext::instance();
      const bool ok = mgr.ok_to_run();
      const auto st = mgr.current_state();
      set_output(0, ok);
      set_output(1, std::string(avs::safety::to_string(st)));
    }

    // Describe outputs for the editor (pseudo-API; adjust to actual graph API as needed)
    std::vector<avs::graph::PortDesc> outputs() const override {
      using PD = avs::graph::PortDesc;
      return { PD{"ok_to_run", avs::graph::Type::Bool}, PD{"state", avs::graph::Type::String} };
    }
  };

  AVS_REGISTER_NODE(EStopStatusNode);

  }} // namespace avs::nodes
#else
  // Fallback: build without graph integration.
#endif
