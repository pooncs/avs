cmake_minimum_required(VERSION 3.21)

add_library(avs_adapter_abb
    src/abb_client.cpp
)

target_include_directories(avs_adapter_abb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(WITH_ABB)
    find_package(ABBPCSDK QUIET)
    if(ABBPCSDK_FOUND)
        target_compile_definitions(avs_adapter_abb PUBLIC AVS_HAVE_ABB_PCSDK=1)
        target_include_directories(avs_adapter_abb PUBLIC ${ABBPCSDK_INCLUDE_DIRS})
        target_link_libraries(avs_adapter_abb PUBLIC ${ABBPCSDK_LIBRARIES})
    else()
        target_compile_definitions(avs_adapter_abb PUBLIC AVS_HAVE_ABB_PCSDK=0)
    endif()
else()
    target_compile_definitions(avs_adapter_abb PUBLIC AVS_HAVE_ABB_PCSDK=0)
endif()

# deps
target_link_libraries(avs_adapter_abb PUBLIC avs_core avs_graph)
