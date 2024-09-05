@set BUILD_DIR="./build"
@set VCPKG_DIR="D:\tools\vcpkg"

del %BUILD_DIR% /Q /S

cmake -B %BUILD_DIR% -S . "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake"
cmake --build %BUILD_DIR% --config "Release" --target ALL_BUILD

@pause