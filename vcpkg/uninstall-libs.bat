@REM Add vcpkg to PATH or uncomment line bellow
@REM cd C:/tools/vcpkg

vcpkg remove librdkafka:x64-windows
vcpkg remove avro-cpp:x64-windows
vcpkg remove boost-property-tree:x64-windows
vcpkg remove boost-json:x64-windows
vcpkg remove snappy:x64-windows
vcpkg remove gtest

pause