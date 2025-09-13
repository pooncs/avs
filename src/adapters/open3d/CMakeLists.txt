# AVS adapters: Open3D
cmake_minimum_required(VERSION 3.16)

if(NOT DEFINED WITH_OPEN3D)
    option(WITH_OPEN3D "Build with Open3D" OFF)
endif()

if(NOT WITH_OPEN3D)
    return()
endif()

add_library(avs_open3d_adapter
    src/Open3DAdapter.cpp
)

# Public headers
set_target_properties(avs_open3d_adapter PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_include_directories(avs_open3d_adapter
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(Open3D CONFIG REQUIRED)

target_link_libraries(avs_open3d_adapter
    PUBLIC Open3D::Open3D
)

# Windows: enable highest warning level for MSVC
if(MSVC)
    target_compile_options(avs_open3d_adapter PRIVATE /W4 /permissive-)
else()
    target_compile_options(avs_open3d_adapter PRIVATE -Wall -Wextra -Wpedantic)
endif()
