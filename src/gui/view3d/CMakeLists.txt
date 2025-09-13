cmake_minimum_required(VERSION 3.16)

add_library(avs_gui_view3d)

# Public API
target_include_directories(avs_gui_view3d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# Sources
set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/gui/view3d/Viewer3DOpen3D.cpp
)

if(NOT WITH_OPEN3D)
    list(APPEND SRC ${CMAKE_CURRENT_SOURCE_DIR}/../../src/gui/view3d/Viewer3DNull.cpp)
endif()

target_sources(avs_gui_view3d PRIVATE ${SRC})

if(WITH_OPEN3D)
    find_package(Open3D CONFIG QUIET)
    if(Open3D_FOUND)
        target_compile_definitions(avs_gui_view3d PRIVATE WITH_OPEN3D)
        target_link_libraries(avs_gui_view3d PRIVATE Open3D::Open3D)
    else()
        message(WARNING "WITH_OPEN3D=ON but Open3D not found. Building stub implementation.")
        target_sources(avs_gui_view3d PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../src/gui/view3d/Viewer3DNull.cpp)
    endif()
endif()

set_target_properties(avs_gui_view3d PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
