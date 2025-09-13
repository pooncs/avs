# AVS GUI module

option(WITH_IMGUI "Enable Dear ImGui + ImNodes GUI" ON)
set(IMGUI_DIR "${BASE_PATH}/3rdParty/imgui")
set(IMNODES_DIR "${BASE_PATH}/3rdParty/imnodes")

add_library(avs_gui)

# Public headers
target_include_directories(avs_gui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Core sources
target_sources(avs_gui
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/GuiFactory.cpp
)

if(WITH_IMGUI)
    target_sources(avs_gui PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/imgui/ImGuiGui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/gui/imgui/ImGuiGraphEditor.cpp
    )
    target_compile_definitions(avs_gui PRIVATE AVS_WITH_IMGUI=1)

    if(IMGUI_DIR)
        target_include_directories(avs_gui PRIVATE ${IMGUI_DIR})
    endif()
    if(IMNODES_DIR)
        target_include_directories(avs_gui PRIVATE ${IMNODES_DIR})
    endif()
endif()

if(TARGET avs_gui)
    target_sources(avs_gui PRIVATE src/metrics_panel.cpp)
    target_include_directories(avs_gui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
else()
    add_library(avs_gui src/metrics_panel.cpp)
    target_include_directories(avs_gui PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
    target_link_libraries(avs_gui PUBLIC avs_core)
endif()
# Consumers should link avs_gui
