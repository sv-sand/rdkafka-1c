#!/bin/bash

# Add vcpkg to PATH or uncomment line bellow
# cd etc/vcpkg/vcpkg

vcpkg remove librdkafka:x64-linux
vcpkg remove boost-test:x64-linux
vcpkg remove boost-property-tree:x64-linux
vcpkg remove boost-json:x64-linux
vcpkg remove snappy:x64-linux
vcpkg remove gtest

pause