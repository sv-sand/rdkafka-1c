BUILD_DIR="./build"
VCPKG_DIR="/home/sand/tools/vcpkg"

rm -rf $BUILD_DIR

cmake -B $BUILD_DIR -S . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake
cmake --build $BUILD_DIR --config "Release"

@pause