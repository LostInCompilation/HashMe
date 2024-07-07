#!/bin/bash

# Create build directory
mkdir -p build
cd build/

# Build project with 4 threads
#make -j4

# Build project with CMake (4 threads)
cmake --build . -j4

# Go back to project root
cd ..
