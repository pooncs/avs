#include "avs/gui/GuiFactory.h"

namespace avs::gui {
#if defined(AVS_WITH_IMGUI)
std::unique_ptr<IGui> CreateImGuiGui(); // implemented in ImGui backend
#endif

std::unique_ptr<IGui> CreateGui() {
#if defined(AVS_WITH_IMGUI)
    return CreateImGuiGui();
#else
    return {};
#endif
}

} // namespace avs::gui
