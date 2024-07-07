#!/bin/sh

BUILD_TYPE=$1
FRESH_BUILD=$2

# Do we have a build type?
if [ -z $BUILD_TYPE ]
then
	echo "No build type specified. Using Release as default."
	BUILD_TYPE="Release"
fi

if [[ $FRESH_BUILD == "fresh" ]]
then
	echo "Making a fresh build"
	
	rm -rf build/*
	
	cmake -S . -B build/ --fresh -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_COLOR_DIAGNOSTICS=ON
else
	cmake -S . -B build/ -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DCMAKE_COLOR_DIAGNOSTICS=ON
fi
