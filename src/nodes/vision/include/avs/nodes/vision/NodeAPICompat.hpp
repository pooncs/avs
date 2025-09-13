#pragma once
#if __has_include(<avs/graph/Registry.hpp>)
  #include <avs/graph/Registry.hpp>
  #define AVS_HAS_REGISTRY 1
#else
  #define AVS_HAS_REGISTRY 0
  // Fallback no-op macro
  #ifndef AVS_REGISTER_NODE
  #define AVS_REGISTER_NODE(T)
  #endif
#endif
