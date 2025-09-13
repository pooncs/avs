#pragma once
#include <string>
#include <vector>

namespace avs { namespace core { class TelemetryRegistry; }}

namespace avs { namespace gui {

// Minimal metrics panel API; console fallback.
class IMetricsPanel {
public:
    virtual ~IMetricsPanel() = default;
    virtual void render() = 0; // paint once; caller drives frequency
};

// Factory creates a console panel that prints metrics snapshot.
IMetricsPanel* create_console_metrics_panel(); // caller takes ownership (managed by unique_ptr at call site)

}} // namespace
