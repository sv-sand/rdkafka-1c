@set BUILD_DIR=".\build"
@set VCPKG_DIR="D:\tools\vcpkg"

del %BUILD_DIR% /Q /S

cmake -S . -B %BUILD_DIR% --toolchain %VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake
cmake --build %BUILD_DIR% --config "Release" --target RdKafka1C

@pause