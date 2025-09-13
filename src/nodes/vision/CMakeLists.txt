# Vision nodes (OpenCV-backed)
cmake_minimum_required(VERSION 3.18)

if(WITH_OPENCV)
  find_package(OpenCV REQUIRED core imgproc highgui)
  add_library(avs_nodes_vision STATIC
    src/GrayscaleNode.cpp
    src/BlurNode.cpp
    src/CannyNode.cpp
    src/TemplateMatchNode.cpp
    src/RegisterVisionNodes.cpp)
  target_include_directories(avs_nodes_vision
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
  target_link_libraries(avs_nodes_vision PUBLIC avs_adapters_opencv ${OpenCV_LIBS})
  target_compile_features(avs_nodes_vision PUBLIC cxx_std_17)
  target_compile_definitions(avs_nodes_vision PUBLIC WITH_OPENCV)
endif()
