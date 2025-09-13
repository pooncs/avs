
# Photoneo adapter

if (WITH_PHOTONEO)
    find_package(PhoXi REQUIRED)
endif()

add_library(avs_adapters_photoneo
    src/PhoxiDevice.cpp
)

add_library(AVS::adapters_photoneo ALIAS avs_adapters_photoneo)

target_include_directories(avs_adapters_photoneo
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(avs_adapters_photoneo PUBLIC cxx_std_17)

target_compile_definitions(avs_adapters_photoneo PUBLIC $<$<BOOL:${WITH_PHOTONEO}>:WITH_PHOTONEO>)

if (WITH_PHOTONEO)
    target_link_libraries(avs_adapters_photoneo PUBLIC PhoXi::API)
endif()
