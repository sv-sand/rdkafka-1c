#!/bin/bash

# Add vcpkg to PATH or uncomment line bellow
# cd etc/vcpkg/vcpkg

vcpkg install librdkafka --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-test --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-property-tree --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-json --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install snappy --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install gtest

pause