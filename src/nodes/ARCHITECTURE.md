# Thread 5: OpenCV adapter + basic vision nodes

## Modules

### `adapters/opencv`
Small adapter around OpenCV providing:
- Availability/version query.
- Safe `cv::Mat` views from external memory and blob export.
- Channel helpers (`ensureGray`, `ensureBGR`).

Guarded by `WITH_OPENCV`. Target: `avs_adapters_opencv`.

### `nodes/vision`
OpenCV-backed nodes:
- `vision.grayscale` → grayscale conversion.
- `vision.blur` → Gaussian blur.
- `vision.canny` → edge detector.
- `vision.template_match` → returns score map and best match.

Nodes are header-only interfaces with `.cpp` implementations.
Registration uses `AVS_REGISTER_NODE` if available (no-op fallback).

Guarded by `WITH_OPENCV`. Target: `avs_nodes_vision`.

## CMake wiring
Add to your root `CMakeLists.txt` (or an included `.cmake` file):

```cmake
option(WITH_OPENCV "Build with OpenCV" ON)

if(WITH_OPENCV)
  add_subdirectory(adapters/opencv)
  add_subdirectory(nodes/vision)
  # Link into your app if desired:
  # target_link_libraries(avs_app PRIVATE avs_nodes_vision)
endif()
```

## Notes
- C++17, RAII, no `new/delete`.
- No exceptions across module boundaries.
- Minimal includes; registration is optional to keep `avs_app` building even if the registry header isn't present yet.
