@set BUILD_DIR="./build"

cmake -S . -B %BUILD_DIR% "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" -DVCPKG_TARGET_TRIPLET=x64-windows-static-md

@pause