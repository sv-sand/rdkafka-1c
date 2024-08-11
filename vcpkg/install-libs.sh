#!/bin/bash

# Before add vcpkg to system PATH

vcpkg install librdkafka --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-test --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-property-tree --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-json --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-uuid --overlay-triplets=static-triplets --triplet x64-windows
vcpkg install snappy --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install gtest

pause