
# FindPhoXi.cmake - locates Photoneo PhoXi API
# Sets: PHOXI_FOUND, PHOXI_INCLUDE_DIR, PHOXI_LIBRARY
# Provides imported target: PhoXi::API

find_path(PHOXI_INCLUDE_DIR PhoXi.h
    HINTS
        $ENV{PHOXI_SDK_DIR}/API
        $ENV{PHOXI_SDK_DIR}/include
        "C:/Program Files/Photoneo/PhoXiControl/SDK/API"
        "C:/Program Files/Photoneo/PhoXiControl/API"
        "C:/Program Files/Photoneo/PhoXiControl/include"
        /opt/photoneo/PhoXiControl/SDK/API
        /opt/photoneo/PhoXiControl/API
)

find_library(PHOXI_LIBRARY NAMES PhoXi_API PhoXi_API_msvc14_Release PhoXi_API_msvc141_Release PhoXi_API_msvc142_Release
    HINTS
        $ENV{PHOXI_SDK_DIR}/lib
        "C:/Program Files/Photoneo/PhoXiControl/SDK/API"
        "C:/Program Files/Photoneo/PhoXiControl/API"
        /opt/photoneo/PhoXiControl/SDK/API
        /opt/photoneo/PhoXiControl/API
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(PhoXi DEFAULT_MSG PHOXI_INCLUDE_DIR PHOXI_LIBRARY)

if (PhoXi_FOUND)
    add_library(PhoXi::API UNKNOWN IMPORTED)
    set_target_properties(PhoXi::API PROPERTIES
        IMPORTED_LOCATION "${PHOXI_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${PHOXI_INCLUDE_DIR}"
    )
endif()
