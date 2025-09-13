# OpenCV adapter
cmake_minimum_required(VERSION 3.18)

if(WITH_OPENCV)
  find_package(OpenCV REQUIRED core imgproc)
  add_library(avs_adapters_opencv STATIC
    src/OpenCVAdapter.cpp)
  target_include_directories(avs_adapters_opencv
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(avs_adapters_opencv PUBLIC ${OpenCV_LIBS})
  target_compile_features(avs_adapters_opencv PUBLIC cxx_std_17)
  target_compile_definitions(avs_adapters_opencv PUBLIC WITH_OPENCV AVS_WITH_OPENCV_HEADERS=1)
endif()
