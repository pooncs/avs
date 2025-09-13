cmake_minimum_required(VERSION 3.20)
project(avs_plugin_loader LANGUAGES CXX)

add_library(avs_plugin_loader STATIC
    src/plugin_loader.cpp)

target_include_directories(avs_plugin_loader PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:include>)

if (BUILD_SHARED_LIBS)
  target_compile_definitions(avs_plugin_loader PRIVATE AVS_BUILD_SHARED)
else()
  target_compile_definitions(avs_plugin_loader PUBLIC AVS_USE_SHARED)
endif()

set_property(TARGET avs_plugin_loader PROPERTY CXX_STANDARD 17)
set_property(TARGET avs_plugin_loader PROPERTY CXX_STANDARD_REQUIRED ON)
