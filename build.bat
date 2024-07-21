@set BUILD_DIR="./build"

del %BUILD_DIR% /Q /S

cmake -B %BUILD_DIR% -S . "-DCMAKE_TOOLCHAIN_FILE=C:\tools\vcpkg\scripts\buildsystems\vcpkg.cmake"
cmake --build %BUILD_DIR% --config "Release" --target ALL_BUILD

@pause