
cmake_minimum_required(VERSION 3.20)
project(AutoVisionStudio LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(3RDPARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdParty")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(BASE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

set(Open3D_DIR "${3RDPARTY_DIR}/open3d/CMake")
set(OpenCV_DIR "${3RDPARTY_DIR}/opencv/")
set(Imalligent_DIR "${3RDPARTY_DIR}/Imalligent/")

option(WITH_OPENCV "Enable OpenCV" ON)
option(WITH_OPEN3D "Enable Open3D" OFF)
option(WITH_REALSENSE "Enable Intel RealSense" OFF)
option(WITH_PHOTONEO "Enable Photoneo" OFF)
option(WITH_CREAFORM "Enable Creaform" OFF)
option(WITH_ABB "Enable ABB PC SDK" OFF)
option(WITH_PYTHON "Embed CPython" OFF)
option(WITH_QT "Build Qt GUI" OFF)
option(WITH_IMGUI "Enable Dear ImGui GUI" ON)

add_subdirectory(src)
add_subdirectory(docs)

if(WITH_TESTS)
  add_subdirectory(tests)
endif()

include(CTest)
