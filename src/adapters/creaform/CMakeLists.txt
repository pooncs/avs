
cmake_minimum_required(VERSION 3.20)

project(avs_adapters_creaform LANGUAGES CXX)

option(WITH_CREAFORM "Build Creaform adapter and nodes" ON)

if(NOT WITH_CREAFORM)
    message(STATUS "WITH_CREAFORM=OFF: skipping Creaform adapter")
    return()
endif()

add_library(avs_adapters_creaform
    src/CreaformAdapter.cpp
)

add_library(avs::adapters::creaform ALIAS avs_adapters_creaform)

# Public headers
 target_include_directories(avs_adapters_creaform
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
 )

# Language standard and warnings
 target_compile_features(avs_adapters_creaform PUBLIC cxx_std_17)
 if(MSVC)
    target_compile_options(avs_adapters_creaform PRIVATE /W4 /permissive-)
 else()
    target_compile_options(avs_adapters_creaform PRIVATE -Wall -Wextra -Wpedantic)
 endif()

# Optional Open3D
option(WITH_OPEN3D "Enable Open3D integration" OFF)
if(WITH_OPEN3D)
    find_package(Open3D QUIET CONFIG)
    if(Open3D_FOUND)
        message(STATUS "Creaform adapter: Open3D found: ${Open3D_VERSION}")
        target_link_libraries(avs_adapters_creaform PUBLIC Open3D::Open3D)
        target_compile_definitions(avs_adapters_creaform PUBLIC WITH_OPEN3D)
    else()
        message(WARNING "Open3D not found: building Creaform adapter without Open3D. Mesh I/O limited.")
    endif()
endif()

# Export interface definition for external discovery
 target_compile_definitions(avs_adapters_creaform PUBLIC WITH_CREAFORM)

