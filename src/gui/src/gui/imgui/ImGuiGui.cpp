#include "ImGuiGui.h"

// Dear ImGui + ImNodes
#include <imgui.h>
#include <imnodes.h>

namespace avs::gui {

ImGuiGui::ImGuiGui() = default;
ImGuiGui::~ImGuiGui() { shutdown(); }

bool ImGuiGui::init(const GuiConfig& cfg) {
    cfg_ = cfg;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImNodes::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    if (cfg.enableDocking) io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#ifdef IMGUI_HAS_VIEWPORT
    if (cfg.enableViewports) io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
    if (!cfg.iniPath.empty()) io.IniFilename = cfg.iniPath.c_str();

    // Style
    ImGui::StyleColorsDark();
    
    editor_ = std::make_unique<avs::gui::ImGuiGraphEditor>();
    initialized_ = true;
    return true;
}

void ImGuiGui::setGraph(std::shared_ptr<avs::graph::Graph> g) {
    graph_ = std::move(g);
    if (editor_) editor_->setGraph(graph_);
}

void ImGuiGui::newFrame() {
    if (!initialized_) return;
    ImGui::NewFrame();
}

void ImGuiGui::drawDockspace_() {
    if (!cfg_.enableDocking) return;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("AVS DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("AVS_DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    drawMainMenu_();

    ImGui::End();
}

void ImGuiGui::drawMainMenu_() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit")) {
                // no-op here
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Graph", nullptr, true, true);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void ImGuiGui::render() {
    if (!initialized_) return;

    drawDockspace_();

    // Graph window
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Graph")) {
        if (editor_) editor_->draw("GraphEditor");
    }
    ImGui::End();

    ImGui::Render();
    // NOTE: actual rendering to platform/graphics backend is app-owned.
}

void ImGuiGui::shutdown() {
    if (!initialized_) return;
    editor_.reset();
    ImNodes::DestroyContext();
    ImGui::DestroyContext();
    initialized_ = false;
}

std::unique_ptr<IGui> CreateImGuiGui() { return std::make_unique<ImGuiGui>(); }

} // namespace avs::gui
