
add_subdirectory(core)
add_subdirectory(util)
add_subdirectory(graph)
add_subdirectory(runtime)
add_subdirectory(scripting)
add_subdirectory(adapters)
add_subdirectory(nodes)
add_subdirectory(app)
add_subdirectory(gui)
add_subdirectory(plugins)

if(WITH_OPENCV)
  add_subdirectory(adapters/opencv)
  add_subdirectory(nodes/vision)
  # target_link_libraries(avs_app PRIVATE avs_nodes_vision)
endif()

if(WITH_OPEN3D)
    add_subdirectory(adapters/open3d)
    add_subdirectory(nodes/pointcloud)
    target_link_libraries(avs_app PRIVATE avs_nodes_pointcloud)
endif()

if (WITH_REALSENSE)
    add_subdirectory(adapters/realsense)
    add_subdirectory(nodes/realsense)
    target_link_libraries(avs_app PRIVATE avs_nodes_realsense)
endif()

if (WITH_PYTHON)
    add_subdirectory(modules/scripting)
    add_subdirectory(modules/nodes/scripting)
    target_link_libraries(avs_app PRIVATE avs_scripting avs_nodes_scripting)
endif()


option(WITH_TESTS "Build unit tests" ON)
option(WITH_CATCH2 "Use Catch2 for tests" ON)
option(WITH_GTEST "Use GoogleTest for tests" OFF)

include(FetchContent)

if(WITH_TESTS)
  enable_testing()
  if(WITH_CATCH2)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.6.0
    )
    FetchContent_MakeAvailable(Catch2)
  endif()
  if(WITH_GTEST)
    FetchContent_Declare(
      googletest
      GIT_REPOSITORY https://github.com/google/googletest.git
      GIT_TAG v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
  endif()
endif()

add_subdirectory(templates)
# Link into app if present
if(TARGET avs_app)
  target_link_libraries(avs_app PRIVATE avs_templates)
endif()