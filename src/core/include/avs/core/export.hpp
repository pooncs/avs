#pragma once

// AVS export/import macro for Windows DLLs
#if defined(_WIN32) || defined(_WIN64)
  #if defined(AVS_BUILD_SHARED)
    #define AVS_API __declspec(dllexport)
  #elif defined(AVS_USE_SHARED)
    #define AVS_API __declspec(dllimport)
  #else
    #define AVS_API
  #endif
#else
  #define AVS_API
#endif

