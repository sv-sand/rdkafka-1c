@set BUILD_DIR="./build"
@set VCPKG_DIR=D:\tools\vcpkg

cmake -B %BUILD_DIR% -S . "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake"

@pause