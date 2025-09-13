# Runtime module CMake
cmake_minimum_required(VERSION 3.20)

option(WITH_GRPC "Enable gRPC control server" OFF)
option(WITH_REST "Enable REST control server" OFF)
option(WITH_WINDOWS_SERVICE "Enable Windows service host" ON)

set(AVS_RUNTIME_HEADLESS_SOURCES
    src/HeadlessExecutor.cpp
    src/ServiceHost.cpp
    src/rest/RestServer.cpp
    src/grpc/GrpcServer.cpp
)

set(AVS_RUNTIME_HEADLESS_HEADERS
    include/avs/runtime/HeadlessExecutor.h
    include/avs/runtime/IControlServer.h
    include/avs/runtime/ServiceHost.h
    include/avs/runtime/rest/RestServer.h
    include/avs/runtime/grpc/GrpcServer.h
)

add_library(avs_runtime
    src/job_system.cpp
)

add_library(avs_runtime_headless STATIC ${AVS_RUNTIME_HEADLESS_SOURCES} ${AVS_RUNTIME_HEADLESS_HEADERS})
add_library(avs::runtime_headless ALIAS avs_runtime_headless)

set_target_properties(avs_runtime_headless PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

target_include_directories(avs_runtime_headless
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include
)

if (WITH_REST)
    target_compile_definitions(avs_runtime_headless PUBLIC AVS_WITH_REST)
    # TODO: link to REST deps (cpprestsdk or Boost.Beast) when available.
endif()

if (WITH_GRPC)
    target_compile_definitions(avs_runtime_headless PUBLIC AVS_WITH_GRPC)
    # TODO: link to gRPC libs when available.
endif()

if (WIN32 AND WITH_WINDOWS_SERVICE)
    target_compile_definitions(avs_runtime_headless PUBLIC AVS_WITH_WIN_SERVICE)
    target_link_libraries(avs_runtime_headless PRIVATE ws2_32)
endif()

target_compile_features(avs_runtime PUBLIC cxx_std_17)

find_package(Threads REQUIRED)

target_link_libraries(avs_runtime PUBLIC Threads::Threads)