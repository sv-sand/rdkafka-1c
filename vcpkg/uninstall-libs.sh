#!/bin/bash

# Before add vcpkg to system PATH

vcpkg remove librdkafka:x64-linux
vcpkg remove boost-test:x64-linux
vcpkg remove boost-property-tree:x64-linux
vcpkg remove boost-json:x64-linux
vcpkg remove boost-uuid:x64-linux
vcpkg remove snappy:x64-linux
vcpkg remove openssl:x64-linux
vcpkg remove gtest

pause