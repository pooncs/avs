#pragma once

#include <memory>
#include "avs/gui/GraphEditor.h"

struct ImNodesContext; // fwd from imnodes

namespace avs { namespace graph { class Graph; } }

namespace avs::gui {

class ImGuiGraphEditor final : public IGraphEditor {
public:
    ImGuiGraphEditor();
    ~ImGuiGraphEditor() override;

    void setGraph(std::shared_ptr<avs::graph::Graph> g) override;
    void draw(const char* id) override;

private:
    std::shared_ptr<avs::graph::Graph> graph_{};
};

} // namespace avs::gui