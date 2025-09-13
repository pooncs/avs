conan install . -s compiler.cppstd=17 --output-folder=build --build=missing --settings=build_type=Release
cmake --preset conan-default
cmake --build build --config Release