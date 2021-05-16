#!/usr/bin/env bash

# Use this to build bounce on any system from a bash shell
rm -rf build
mkdir build
cd build
cmake -DBOUNCE_BUILD_DOCS=OFF ..
cmake --build .