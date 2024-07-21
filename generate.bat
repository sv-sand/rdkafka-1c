@set BUILD_DIR="./build"

cmake -B %BUILD_DIR% -S . "-DCMAKE_TOOLCHAIN_FILE=C:\tools\vcpkg\scripts\buildsystems\vcpkg.cmake"

@pause