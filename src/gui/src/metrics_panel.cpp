#include "avs/gui/metrics_panel.hpp"
#include "avs/core/telemetry.hpp"
#include "avs/core/logging.hpp"
#include <memory>
#include <iostream>
#include <iomanip>

namespace avs { namespace gui {

class ConsoleMetricsPanel : public IMetricsPanel {
public:
    void render() override {
        auto snap = avs::core::TelemetryRegistry::instance().snapshot();
        std::cout << "=== Metrics Snapshot ===\n";
        for(const auto& m: snap){
            if(m.kind=="counter") std::cout << m.name << ": " << m.i1 << " (counter)\n";
            else if(m.kind=="gauge") std::cout << m.name << ": " << m.v1 << " (gauge)\n";
            else if(m.kind=="timer") std::cout << m.name << ": avg_ms=" << std::fixed << std::setprecision(3) << m.v1 << ", count=" << m.i1 << " (timer)\n";
        }
    }
};

IMetricsPanel* create_console_metrics_panel(){ return new ConsoleMetricsPanel(); }

}} // namespace
