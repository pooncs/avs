# Try to find ABB PC SDK
# Usage: set(ABB_SDK_DIR "C:/Program Files/ABB/PC SDK") before find_package(ABBPCSDK)

find_path(ABBPCSDK_INCLUDE_DIR NAMES abb/robotsdk.h PATHS ${ABB_SDK_DIR}/include NO_DEFAULT_PATH)
find_library(ABBPCSDK_LIBRARY NAMES ABB.Robotics.Controllers
    PATHS ${ABB_SDK_DIR}/lib ${ABB_SDK_DIR}/lib/msc
    NO_DEFAULT_PATH)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ABBPCSDK REQUIRED_VARS ABBPCSDK_INCLUDE_DIR ABBPCSDK_LIBRARY)

if(ABBPCSDK_FOUND)
    set(ABBPCSDK_INCLUDE_DIRS ${ABBPCSDK_INCLUDE_DIR})
    set(ABBPCSDK_LIBRARIES ${ABBPCSDK_LIBRARY})
endif()
