
add_library(avs_graph
  src/graph.cpp
  src/executor.cpp
  src/registry.cpp
  src/serialization.cpp
)
target_include_directories(avs_graph PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(avs_graph PUBLIC avs_core avs_util)
