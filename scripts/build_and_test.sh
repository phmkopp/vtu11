#!/bin/bash

# to be executed from the root-dir

cmake -H"." -DVTU11_ENABLE_TESTS=ON -B"build"
cmake --build "build"

cd build # tests need to be executed in the same dir as the test_runner, otherwise path are not working correctly
./vtu11_testrunner