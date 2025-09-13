cmake_minimum_required(VERSION 3.21)
add_library(avs_nodes_robotics
    src/abb_read_rapid.cpp
    src/abb_write_rapid.cpp
    src/abb_movej.cpp
    src/abb_movel.cpp
)

target_include_directories(avs_nodes_robotics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(avs_nodes_robotics PUBLIC avs_core avs_graph avs_adapter_abb)
