# AVS nodes: pointcloud, powered by Open3D adapter
cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED WITH_OPEN3D)
    option(WITH_OPEN3D "Build with Open3D" OFF)
endif()

if(NOT WITH_OPEN3D)
    return()
endif()

add_library(avs_nodes_pointcloud
    src/VoxelDownsampleNode.cpp
    src/ICPRegistrationNode.cpp
    src/PlaneSegmentationNode.cpp
)

set_target_properties(avs_nodes_pointcloud PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_include_directories(avs_nodes_pointcloud
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(avs_nodes_pointcloud
    PUBLIC avs_open3d_adapter
)

if(MSVC)
    target_compile_options(avs_nodes_pointcloud PRIVATE /W4 /permissive-)
else()
    target_compile_options(avs_nodes_pointcloud PRIVATE -Wall -Wextra -Wpedantic)
endif()
