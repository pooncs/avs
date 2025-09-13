
# Photoneo nodes
add_library(avs_nodes_photoneo
    src/PhoxiGrabNode.cpp
)

add_library(AVS::nodes_photoneo ALIAS avs_nodes_photoneo)

# Expect parent to provide AVS::core and AVS::graph

target_include_directories(avs_nodes_photoneo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(avs_nodes_photoneo PUBLIC cxx_std_17)

target_link_libraries(avs_nodes_photoneo
    PUBLIC
        AVS::adapters_photoneo
        AVS::core
        AVS::graph
)
