# AVS scripting module (pybind11 embedding)

option(WITH_PYTHON "Enable CPython embedding via pybind11" OFF)

if (WITH_PYTHON)
    find_package(pybind11 CONFIG QUIET)
    if (NOT pybind11_FOUND)
        message(FATAL_ERROR "WITH_PYTHON=ON but pybind11 not found. Please provide pybind11 (CONFIG).")
    endif()

    add_library(avs_scripting
        src/PyEngine.cpp
        src/PyBindings.cpp)
    target_include_directories(avs_scripting PUBLIC include)
    target_link_libraries(avs_scripting PUBLIC pybind11::embed)
    target_compile_definitions(avs_scripting PUBLIC WITH_PYTHON)
    set_target_properties(avs_scripting PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

    # Example install rules (optional)
    # install(TARGETS avs_scripting EXPORT avsTargets)
endif()
