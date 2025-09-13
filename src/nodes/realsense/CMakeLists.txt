cmake_minimum_required(VERSION 3.19)
project(avs_nodes_realsense LANGUAGES CXX)

add_library(avs_nodes_realsense
    src/RSColorNode.cpp
    src/RSDepthNode.cpp)

target_include_directories(avs_nodes_realsense PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(avs_nodes_realsense PUBLIC cxx_std_17)

target_link_libraries(avs_nodes_realsense PUBLIC avs_adapters_realsense)

# Link to graph/core if present
foreach(tgt IN ITEMS avs_graph avs_core avs_util)
    if (TARGET ${tgt})
        target_link_libraries(avs_nodes_realsense PUBLIC ${tgt})
    endif()
endforeach()
