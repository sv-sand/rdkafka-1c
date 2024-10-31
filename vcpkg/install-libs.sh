#!/bin/bash

# Before add vcpkg to system PATH

vcpkg install boost-filesystem
vcpkg install boost-iostreams
vcpkg install boost-program-options

vcpkg install openssl --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install librdkafka --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-test --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-property-tree --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-json --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install boost-uuid --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install snappy --overlay-triplets=static-triplets --triplet x64-linux
vcpkg install gtest
