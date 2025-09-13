cmake_minimum_required(VERSION 3.19)
project(avs_adapters_realsense LANGUAGES CXX)

add_library(avs_adapters_realsense
    src/RealSenseDeviceManager.cpp
    src/RealSenseFrameGrabber.cpp)

target_include_directories(avs_adapters_realsense PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(avs_adapters_realsense PUBLIC cxx_std_17)

if (WITH_REALSENSE)
    find_package(realsense2 CONFIG QUIET)
    if (realsense2_FOUND)
        target_link_libraries(avs_adapters_realsense PUBLIC realsense2::realsense2)
        target_compile_definitions(avs_adapters_realsense PUBLIC WITH_REALSENSE)
    else()
        message(WARNING "WITH_REALSENSE=ON but librealsense2 not found; building stub")
    endif()
endif()

# Link to core libs if they exist (optional)
foreach(tgt IN ITEMS avs_core avs_util)
    if (TARGET ${tgt})
        target_link_libraries(avs_adapters_realsense PUBLIC ${tgt})
    endif()
endforeach()
