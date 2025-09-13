#pragma once

#include <memory>
#include <string>
#include <vector>
#include "avs/gui/IGui.h"
#include "avs/gui/GraphEditor.h"

namespace avs { namespace graph { class Graph; } }

namespace avs::gui {

class ImGuiGraphEditor; // fwd

class ImGuiGui final : public IGui {
public:
    ImGuiGui();
    ~ImGuiGui() override;

    bool init(const GuiConfig& cfg) override;
    void setGraph(std::shared_ptr<avs::graph::Graph> g) override;
    void newFrame() override;
    void render() override;
    void shutdown() override;

private:
    GuiConfig cfg_{};
    std::shared_ptr<avs::graph::Graph> graph_{};
    std::unique_ptr<IGraphEditor> editor_{};
    bool initialized_{false};

    // Internal helpers
    void drawDockspace_();
    void drawMainMenu_();
};

std::unique_ptr<IGui> CreateImGuiGui();

} // namespace avs::gui
