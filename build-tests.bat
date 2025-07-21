@set BUILD_DIR=".\build"

del %BUILD_DIR% /Q /S

cmake -S . -B %BUILD_DIR% --toolchain %VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
cmake --build %BUILD_DIR% --config "Release" --target ModuleTests

@pause