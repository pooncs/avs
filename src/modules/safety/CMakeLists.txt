
# Safety module
option(WITH_SAFETY "Build Safety (E-stop) module" ON)
if(NOT WITH_SAFETY)
  return()
endif()

add_library(avs_safety STATIC
  src/SafetyManager.cpp
  src/backends/SimBackend.cpp
  src/nodes/EStopStatusNode.cpp
)

target_include_directories(avs_safety
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(avs_safety PUBLIC cxx_std_17)

# No external deps; keep standalone.
target_compile_definitions(avs_safety PRIVATE AVS_SAFETY_BUILD)

# Consumers can link this to get default SafetyContext.
