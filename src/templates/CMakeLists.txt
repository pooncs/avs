
cmake_minimum_required(VERSION 3.18)
project(avs_templates LANGUAGES CXX)

add_library(avs_templates STATIC
    src/template_common.cpp
    src/bin_picking.cpp
    src/depal.cpp
    src/inspection.cpp
)

add_library(avs::templates ALIAS avs_templates)

target_include_directories(avs_templates PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(avs_templates PUBLIC cxx_std_17)

# Options are forwarded from root. No hard deps here.

option(AVS_INSTALL_SAMPLES "Install sample projects" ON)
if(AVS_INSTALL_SAMPLES)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../projects/samples/
            DESTINATION share/avs/projects
            FILES_MATCHING PATTERN "*.avsproj")
endif()

# Try link to app if present.
if(TARGET avs_app)
    target_link_libraries(avs_app PRIVATE avs_templates)
endif()
