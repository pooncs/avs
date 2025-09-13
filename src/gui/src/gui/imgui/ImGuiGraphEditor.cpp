#include "ImGuiGraphEditor.h"
#include <imnodes.h>
#include <imgui.h>

namespace avs::gui {

ImGuiGraphEditor::ImGuiGraphEditor() = default;
ImGuiGraphEditor::~ImGuiGraphEditor() = default;

void ImGuiGraphEditor::setGraph(std::shared_ptr<avs::graph::Graph> g) {
    graph_ = std::move(g);
}

void ImGuiGraphEditor::draw(const char* id) {
    ImNodes::BeginNodeEditor();

    // Placeholder: show a minimal sample if no connected graph.
    ImNodes::BeginNode(1);
    ImNodes::BeginNodeTitleBar();
    ImGui::TextUnformatted("Sample Node");
    ImNodes::EndNodeTitleBar();
    ImNodes::BeginInputAttribute(2);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();
    ImNodes::BeginOutputAttribute(3);
    ImGui::Indent(40);
    ImGui::Text("Out");
    ImNodes::EndOutputAttribute();
    ImNodes::EndNode();

    ImNodes::MiniMap(0.15f, ImNodesMiniMapLocation_TopRight);
    ImNodes::EndNodeEditor();
}

} // namespace avs::gui