# AVS nodes: scripting (Python script node)

option(WITH_PYTHON "Enable CPython embedding via pybind11" OFF)

if (WITH_PYTHON)
    # This target depends on avs_scripting; assume modules/scripting is added first
    if (TARGET avs_scripting)
        add_library(avs_nodes_scripting
            src/PyScriptNode.cpp)
        target_include_directories(avs_nodes_scripting PUBLIC include)
        target_link_libraries(avs_nodes_scripting PUBLIC avs_scripting)
        target_compile_definitions(avs_nodes_scripting PUBLIC WITH_PYTHON)
        set_target_properties(avs_nodes_scripting PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
    else()
        message(STATUS "avs_scripting not found; PyScriptNode will not be built")
    endif()
endif()
