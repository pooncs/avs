#pragma once
#include <string>
#include "avs/core/telemetry.hpp"
#include "avs/core/logging.hpp"

// Example node: increments a counter and logs a heartbeat.
// Integration: registered via AVS_REGISTER_NODE in cpp.

namespace avs { namespace nodes { namespace diagnostics {

class HeartbeatNode {
public:
    struct Params { int period_ms = 1000; };
    explicit HeartbeatNode(const Params& p = {}): p_(p) {}
    void tick(){
        auto& reg = avs::core::TelemetryRegistry::instance();
        reg.counter("diagnostics.heartbeat.count").inc(1);
        avs::core::Logger::instance().info("heartbeat", {{"period_ms", std::to_string(p_.period_ms)}});
    }
private:
    Params p_;
};

}}} // namespace
