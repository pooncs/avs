#pragma once
#include <memory>
#include "avs/gui/IGui.h"

namespace avs::gui {

// Returns a GUI matching build options (e.g., ImGui backend when WITH_IMGUI=ON).
std::unique_ptr<IGui> CreateGui();

} // namespace avs::gui
