
# AutoVisionStudio (AVS)

A C++17 scaffolding for an automotive inspection & automation suite with a node-graph, optional GUI, robotics & vision adapters, and embedded Python.

> This is a scaffold. Most components are stubs to be completed in separate threads.

## Build (Windows/MSVC 2019)
```bat
mkdir build && cd build
cmake -G "Visual Studio 16 2019" -A x64 .. ^
  -DWITH_OPENCV=ON -DWITH_OPEN3D=ON -DWITH_PYTHON=ON -DWITH_QT=OFF -DWITH_IMGUI=ON
cmake --build . --config Release
```
