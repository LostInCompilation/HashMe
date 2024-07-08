#!/bin/bash

################################################################################
# Script for building.														   #
#																			   #
# (C) 2024 Marc Schöndorf													   #
# Licensed under zlib License. See LICENSE.md								   #
################################################################################

# Create build directory
mkdir -p build
cd build/

# Build project with 4 threads
#make -j4

# Build project using CMake build (4 threads)
cmake --build . -j4

# Go back to project root
cd ..
