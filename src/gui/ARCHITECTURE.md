# AVS GUI (Dear ImGui + ImNodes)

This adds an **ImGui** GUI backend with **docking** and a **node graph editor** built on **ImNodes**. The public API is backend-agnostic and intended to be shared with a future Qt implementation.

## Layout
```
include/avs/gui/
  IGui.h            # public GUI interface
  GraphEditor.h     # public graph editor interface
  GuiFactory.h      # backend selection factory
src/gui/
  GuiFactory.cpp    # returns backend instance depending on build options
  imgui/
    ImGuiGui.{h,cpp}
    ImGuiGraphEditor.{h,cpp}
```

## Build
- CMake option: `WITH_IMGUI` (OFF by default).
- If `WITH_IMGUI=ON`, provide paths to third-party headers:
  - `IMGUI_DIR` → folder containing `imgui.h`
  - `IMNODES_DIR` → folder containing `imnodes.h`

Example:
```bash
cmake -DWITH_IMGUI=ON \
      -DIMGUI_DIR=c:/3rdparty/imgui \
      -DIMNODES_DIR=c:/3rdparty/imnodes ..
```

> The ImGui backend only manages **ImGui/ImNodes contexts and widgets**. Platform/renderer bindings (e.g., DX11/Vulkan/GL) remain in the app.

## Usage
```cpp
#include <avs/gui/GuiFactory.h>
#include <avs/gui/IGui.h>

using namespace avs::gui;

auto gui = CreateGui();
if (gui) {
    GuiConfig cfg; cfg.enableDocking = true; cfg.enableViewports = false;
    gui->init(cfg);
    gui->setGraph(myGraphPtr);
    while (running) {
        gui->newFrame();
        gui->render();
        // ... present via your renderer backend ...
    }
    gui->shutdown();
}
```

## API Compatibility w/ Qt
The headers in `include/avs/gui` are the **stable** API that both backends implement. The Qt backend should implement the same interfaces so `CreateGui()` returns a Qt-backed implementation when `WITH_QT` is enabled.

## Notes
- RAII: backends create/destroy ImGui/ImNodes contexts in ctor/dtor.
- No raw `new/delete`; `std::unique_ptr` and `std::shared_ptr` used.
- Guards: all external deps are behind `WITH_IMGUI`.
- `avs_app` linking: link to `avs_gui`; when `WITH_IMGUI=OFF`, `CreateGui()` returns `{}` so the app still builds.

## Future
- Sync `avs::graph::Graph` model to ImNodes (IDs, ports, links).
- Node registry UI (uses `AVS_REGISTER_NODE`).
- Save/Load layouts via `GuiConfig.iniPath`.
```
