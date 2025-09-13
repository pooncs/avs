# Diagnostics nodes (optional submodule)
add_library(avs_nodes_diagnostics
    src/heartbeat_node.cpp
)

target_include_directories(avs_nodes_diagnostics PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

target_link_libraries(avs_nodes_diagnostics PUBLIC avs_core)
