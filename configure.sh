#!/bin/bash

################################################################################
# Script for configuring the build.											   #
#																			   #
# (C) 2024 Marc Schöndorf													   #
# Licensed under zlib License. See LICENSE.md								   #
################################################################################

# Vars
makeFreshBuild=false

################################################################################
# Predefine colors
ITALICGREEN="\033[3;32m"
BOLDYELLOW="\033[1;33m"
ITALICYELLOW="\033[3;33m"
BOLDRED="\033[1;31m"
ITALICRED="\033[3;31m"
BOLD="\033[1m"
ITALIC="\033[3m"
ENDCOLOR="\033[0m"

################################################################################
# Print help text
PrintHelp()
{
	echo
	echo -e "${BOLD}Usage:${ENDCOLOR}"
	echo
	echo    "configure.sh BUILD_TYPE FRESH_BUILD"
	echo
	echo -e "     ${BOLD}BUILD_TYPE:${ENDCOLOR} Debug, Release, RelWithDebInfo"
	echo -e "        If no ${ITALIC}BUILD_TYPE${ENDCOLOR} is specified (left empty), ${ITALIC}Release${ENDCOLOR} will be used as default."
	echo -e "     ${BOLD}FRESH_BUILD:${ENDCOLOR} fresh"
	echo -e "        If ${ITALIC}\"fresh\"${ENDCOLOR} is specified, a fresh build will be made by deleting the build folder."
	echo    "        If nothing is specified, a standard build will be made."
	echo
	echo -e "Pass ${ITALIC}-h${ENDCOLOR} or ${ITALIC}--help${ENDCOLOR} to print this help text."
	echo
}

################################################################################
# Read first and second parameter
BUILD_TYPE=$1
FRESH_BUILD_ARG=$2

# Should we print the help text?
if [[ "$BUILD_TYPE" == "-h" ]] || [[ "$BUILD_TYPE" == "--help" ]]
then
	PrintHelp
	exit 0
fi

# Is there just a fresh build specified with missing build type?
if [[ "$BUILD_TYPE" == "fresh" ]]
then
	echo -e "${ITALICYELLOW}Info: No build type specified. Using ${BOLDYELLOW}Release${ENDCOLOR}${ITALICYELLOW} as default build type.${ENDCOLOR}"
	BUILD_TYPE="Release"
	makeFreshBuild=true
fi

# Do we have a build type argument?
if [[ -z "$BUILD_TYPE" ]]
then
	echo -e "${ITALICYELLOW}Info: No build type specified. Using ${BOLDYELLOW}Release${ENDCOLOR}${ITALICYELLOW} as default build type.${ENDCOLOR}"
	BUILD_TYPE="Release"
fi

# Check if BUILD_TYPE has a valid value
if [[ "$BUILD_TYPE" != "Debug" ]] && [[ "$BUILD_TYPE" != "Release" ]] && [[ "$BUILD_TYPE" != "RelWithDebInfo" ]]
then
	echo -e "${BOLDRED}Error: Invalid BUILD_TYPE specified. Valid options are: ${ENDCOLOR}${ITALICRED}Debug, Release, RelWithDebInfo${ENDCOLOR}"
	exit 1
fi

# Is a fresh build specified?
if [[ "$FRESH_BUILD_ARG" == "fresh" ]]
then
	makeFreshBuild=true
fi

# Run CMake command
if [[ "$makeFreshBuild" == true ]]
then
	echo -e "${ITALICGREEN}Info: Making a fresh build.${ENDCOLOR}"
	
	rm -rf build/*
	cmake -S . -B build/ --fresh -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DCMAKE_COLOR_DIAGNOSTICS=ON
else
	cmake -S . -B build/ -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" -DCMAKE_COLOR_DIAGNOSTICS=ON
fi
