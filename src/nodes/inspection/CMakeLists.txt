
cmake_minimum_required(VERSION 3.20)
project(avs_nodes_inspection LANGUAGES CXX)

option(WITH_CREAFORM "Build Creaform adapter and nodes" ON)
if(NOT WITH_CREAFORM)
    message(STATUS "WITH_CREAFORM=OFF: skipping inspection nodes")
    return()
endif()

add_library(avs_nodes_inspection
    src/CreaformImportNode.cpp
    src/BasicInspectionNodes.cpp
)

add_library(avs::nodes::inspection ALIAS avs_nodes_inspection)

# headers
 target_include_directories(avs_nodes_inspection
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
 )
 target_compile_features(avs_nodes_inspection PUBLIC cxx_std_17)
 if(MSVC)
    target_compile_options(avs_nodes_inspection PRIVATE /W4 /permissive-)
 else()
    target_compile_options(avs_nodes_inspection PRIVATE -Wall -Wextra -Wpedantic)
 endif()

# Link to adapter
 target_link_libraries(avs_nodes_inspection PUBLIC avs::adapters::creaform)

# Propagate macros
 target_compile_definitions(avs_nodes_inspection PUBLIC WITH_CREAFORM)

