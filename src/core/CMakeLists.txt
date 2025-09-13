# Append to existing avs_core target
if(TARGET avs_core)
    target_sources(avs_core PRIVATE src/logging.cpp src/telemetry.cpp)
    target_include_directories(avs_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
else()
    add_library(avs_core
        src/logging.cpp
        src/telemetry.cpp
    )
    target_include_directories(avs_core PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)
endif()
