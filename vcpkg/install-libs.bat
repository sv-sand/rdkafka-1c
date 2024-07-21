@REM Add vcpkg to PATH or uncomment line bellow
@REM cd C:/tools/vcpkg

vcpkg install librdkafka --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install avro-cpp --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install boost-property-tree --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install boost-json --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install snappy --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install gtest

pause