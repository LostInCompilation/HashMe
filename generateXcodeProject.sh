#!/bin/sh

BUILD_TYPE="$1"

# Do we have a build type?
if [ -z $BUILD_TYPE ]
then
	echo "No build type specified. Using Release as default."
	BUILD_TYPE="Release"
fi

cmake -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -G Xcode -T buildsystem=12 -S . -B projects/
