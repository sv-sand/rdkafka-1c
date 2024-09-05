@REM Before add vcpkg to system PATH

vcpkg install openssl --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install librdkafka[ssl] --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install avro-cpp --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install boost-property-tree --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install boost-json --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install boost-uuid --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install snappy --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install gtest

pause