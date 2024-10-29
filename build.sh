BUILD_DIR="./build"
VCPKG_DIR="/home/sand/tools/vcpkg"

rm -rf $BUILD_DIR

cmake -S . -B $BUILD_DIR --toolchain $VCPKG_DIR/scripts/buildsystems/vcpkg.cmake
cmake --build $BUILD_DIR --config "Release" --target RdKafka1C
