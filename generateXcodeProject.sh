#!/bin/bash

################################################################################
# Script for generating Xcode project files.                                   #
#                                                                              #
# (C) 2024 Marc Schöndorf                                                      #
# Licensed under the zlib License. See LICENSE.md                              #
################################################################################

################################################################################
# Predefine colors
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
	echo    "generateXcodeProject.sh BUILD_TYPE"
	echo
	echo -e "     ${BOLD}BUILD_TYPE:${ENDCOLOR} Debug, Release, RelWithDebInfo"
	echo -e "        If no ${ITALIC}BUILD_TYPE${ENDCOLOR} is specified (left empty), ${ITALIC}Release${ENDCOLOR} will be used as default."
	echo
	echo -e "Pass ${ITALIC}-h${ENDCOLOR} or ${ITALIC}--help${ENDCOLOR} to print this help text."
	echo
}

################################################################################
# Read first parameter
BUILD_TYPE="$1"

# Should we print the help text?
if [[ "$BUILD_TYPE" == "-h" ]] || [[ "$BUILD_TYPE" == "--help" ]]
then
	PrintHelp
	exit 0
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

cmake -DCMAKE_CONFIGURATION_TYPES="${BUILD_TYPE}" -G Xcode -T buildsystem=12 -S . -B projects/
