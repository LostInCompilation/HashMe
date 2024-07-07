#!/bin/bash

BUILD_TYPE="$1"

# Do we have a build type?
if [ -z $BUILD_TYPE ]
then
	echo "No build type specified. Using Release as default."
	BUILD_TYPE="Release"
fi

cmake -DCMAKE_CONFIGURATION_TYPES="${BUILD_TYPE}" -G Xcode -T buildsystem=12 -S . -B projects/
