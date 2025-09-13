cmake_minimum_required(VERSION 3.20)
project(avs_pack_sample_nodes LANGUAGES CXX)

add_library(avs_pack_sample_nodes SHARED sample_nodes.cpp)

# Consumer of AVS headers; assumes targets/paths provided by the main repo.
# The pack only needs include dirs to avs headers and links to any required AVS libs.
# Here we use INTERFACE include directories to allow consumption from the top-level.

# In the parent project, make sure to add include directories:
# target_include_directories(avs_pack_sample_nodes PRIVATE ${AVS_INCLUDE_DIRS})

# We define AVS_USE_SHARED so that headers use dllimport.
target_compile_definitions(avs_pack_sample_nodes PRIVATE AVS_USE_SHARED)

# C++17 as per repo guidelines
set_property(TARGET avs_pack_sample_nodes PROPERTY CXX_STANDARD 17)
set_property(TARGET avs_pack_sample_nodes PROPERTY CXX_STANDARD_REQUIRED ON)

# On Windows, set output name and runtime dir
if (WIN32)
    set_target_properties(avs_pack_sample_nodes PROPERTIES
        OUTPUT_NAME "avs_pack_sample_nodes"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
endif()
