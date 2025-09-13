
cmake_minimum_required(VERSION 3.16)
project(avs_calibration LANGUAGES CXX)

# Option is expected to be defined at the super project; provide a local default.
option(WITH_OPENCV "Enable OpenCV-dependent calibration features" OFF)

add_library(avs_calibration
    src/hand_eye.cpp
    src/extrinsics.cpp
)

target_include_directories(avs_calibration
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(avs_calibration PUBLIC cxx_std_17)

if(WITH_OPENCV)
    find_package(OpenCV REQUIRED COMPONENTS core calib3d)
    target_compile_definitions(avs_calibration PUBLIC WITH_OPENCV)
    target_link_libraries(avs_calibration PUBLIC opencv_core opencv_calib3d)
endif()

# Consumers: target_link_libraries(your_target PUBLIC avs_calibration)
