cmake_minimum_required(VERSION 3.16)

add_library(avs_nodes_render)

# Public include
target_include_directories(avs_nodes_render PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# Sources
set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/nodes/render/RenderPointCloudNode.cpp
)

target_sources(avs_nodes_render PRIVATE ${SRC})

target_link_libraries(avs_nodes_render PUBLIC avs_gui_view3d)

if(WITH_OPEN3D)
    find_package(Open3D CONFIG QUIET)
    if(Open3D_FOUND)
        target_compile_definitions(avs_nodes_render PRIVATE WITH_OPEN3D)
        target_link_libraries(avs_nodes_render PRIVATE Open3D::Open3D)
    endif()
endif()

set_target_properties(avs_nodes_render PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
