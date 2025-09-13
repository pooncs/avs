
#include <avs/core/logger.h>
#include <avs/gui/GuiFactory.h>
#include <avs/scripting/python_embed.h>
#include <avs/graph/registry.h>
#include <avs/graph/graph.h>
#include <memory>

using namespace avs::gui;

int main(){
    using namespace avs;
    core::log_info("AVS starting...");
    
    // // Create GUI
    // auto gui = CreateGui();        // returns nullptr if WITH_IMGUI=OFF
    // if (gui) {
    //     GuiConfig cfg; 
    //     cfg.enableDocking   = true;
    //     cfg.enableViewports = false; // set true if you compile ImGui with multi-viewports
    //     gui->init(cfg);
    //     gui->setGraph(myGraphSharedPtr); // your avs::graph::Graph

    //     // Main loop (platform renderer owns swap chain / device)
    //     while (running) {
    //         // Platform backend: ImGui_ImplDX11_NewFrame(); ImGui_ImplWin32_NewFrame(); ...
    //         gui->newFrame();
    //         gui->render();          // builds ImGui draw data
    //         // Platform renderer: ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    //         // Present...
    //     }
    //     gui->shutdown();
    // }


    scripting::Python::init({"avs"});
    graph::Graph g; // TODO: load project
    (void)g;
    scripting::Python::shutdown();
    core::log_info("AVS exiting.");
    return 0;
}
