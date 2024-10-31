BUILD_DIR="./build"
VCPKG_DIR="/home/sand/tools/vcpkg"

rm -rf $BUILD_DIR

cmake -B $BUILD_DIR -S . --toolchain $VCPKG_DIR/scripts/buildsystems/vcpkg.cmake
cmake --build $BUILD_DIR --config "Release" --target RdKafka1C
