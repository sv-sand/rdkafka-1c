BUILD_DIR="./build"

rm -rf $BUILD_DIR

cmake -B $BUILD_DIR -S . --toolchain $VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
