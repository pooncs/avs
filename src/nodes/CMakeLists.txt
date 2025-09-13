
add_library(avs_nodes
  src/math_nodes.cpp
)
target_include_directories(avs_nodes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(avs_nodes PUBLIC avs_core avs_graph)
