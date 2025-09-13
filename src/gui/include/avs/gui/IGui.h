#pragma once
#include <memory>
#include <string>

namespace avs {
namespace graph { class Graph; }
namespace gui {

struct GuiConfig {
    bool enableDocking{true};
    bool enableViewports{false};
    std::string iniPath{}; // optional path for ImGui .ini persistence
};

class IGui {
public:
    virtual ~IGui() = default;
    virtual bool init(const GuiConfig& cfg) = 0;
    virtual void setGraph(std::shared_ptr<avs::graph::Graph> g) = 0;
    virtual void newFrame() = 0;     // call once per frame
    virtual void render() = 0;       // call after newFrame() to draw UI
    virtual void shutdown() = 0;     // free resources
};

} // namespace gui
} // namespace avs
