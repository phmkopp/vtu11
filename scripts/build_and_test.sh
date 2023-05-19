#!/bin/bash

# to be executed from the root-dir

cmake -H"." -B"build"
cmake --build "build"  -- -j$(nproc)

cd build # tests need to be executed in the same dir as the test_runner, otherwise path are not working correctly
ctest -T memcheck
