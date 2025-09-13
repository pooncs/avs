
cmake_minimum_required(VERSION 3.16)

option(WITH_CALIBRATION_NODES "Build calibration nodes (requires graph + OpenCV)" OFF)

if (WITH_CALIBRATION_NODES)
    add_library(avs_nodes_calibration
        src/hand_eye_node.cpp
        src/register_nodes.cpp
    )
    target_include_directories(avs_nodes_calibration PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../calibration/include>
    )
    target_compile_features(avs_nodes_calibration PUBLIC cxx_std_17)
    target_link_libraries(avs_nodes_calibration PUBLIC avs_calibration)
    target_compile_definitions(avs_nodes_calibration PUBLIC WITH_CALIBRATION_NODES)
endif()
