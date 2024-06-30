#!/bin/sh

# Create build directory
mkdir -p build
cd build/

# Build project with 8 threads
make -j8

# Build project with CMake
#cmake --build . --target MyExe

# Go back to project root
cd ..
