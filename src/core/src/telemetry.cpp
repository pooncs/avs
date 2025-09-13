#include "avs/core/telemetry.hpp"

namespace avs { namespace core {

TelemetryRegistry& TelemetryRegistry::instance(){ static TelemetryRegistry g; return g; }

Counter& TelemetryRegistry::counter(const std::string& name){ std::lock_guard<std::mutex> l(mtx_); return counters_[name]; }
Gauge&   TelemetryRegistry::gauge  (const std::string& name){ std::lock_guard<std::mutex> l(mtx_); return gauges_[name]; }
Timer&   TelemetryRegistry::timer  (const std::string& name){ std::lock_guard<std::mutex> l(mtx_); return timers_[name]; }

std::vector<TelemetryRegistry::MetricValue> TelemetryRegistry::snapshot(){
    std::lock_guard<std::mutex> l(mtx_);
    std::vector<MetricValue> out; out.reserve(counters_.size()+gauges_.size()+timers_.size());
    for(auto& kv: counters_){ out.push_back({kv.first, "counter", 0.0, 0.0, kv.second.value.load()}); }
    for(auto& kv: gauges_  ){ out.push_back({kv.first, "gauge", kv.second.value.load(), 0.0, 0}); }
    for(auto& kv: timers_  ){ out.push_back({kv.first, "timer", kv.second.avg_ms(), 0.0, kv.second.count.load()}); }
    return out;
}

}} // namespace
