################################################################################
#                       CMake file for HashMe static lib                       #
#                                                                              #
# (C) 2024 Marc Schöndorf                                                      #
# Licensed under the zlib License. See LICENSE.md                              #
################################################################################

cmake_minimum_required(VERSION 3.25)

###########################################################
# Set options for sub projects
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # Needed to set options of subprojects inside parent project

###########################################################
# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS false) # Don't use GNU++

###########################################################
# Set build output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/debug")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/debug")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release")

###########################################################
# Our project
project("HashMe"
	VERSION 0.1.0
	DESCRIPTION "Modern, hardware accelerated hash library for C++"
	LANGUAGES CXX
)

# Main library
add_library("${PROJECT_NAME}"
	include/picobench.h
	include/predef.h
	include/PlatformConfig.hpp
	include/HashMe.hpp
	include/Utils.hpp
	include/HasherBase.hpp
	include/Base64.hpp
	include/SHA256.hpp
	include/SHA256_Hardware.hpp
	include/SHA224.hpp
	include/SHA224_Hardware.hpp
	include/SHA512.hpp
	include/SHA512_Hardware.hpp
	include/SHA384.hpp
	include/SHA384_Hardware.hpp
	include/MD5.hpp
	include/CRC_Software.hpp
	include/CRC32_Hardware.hpp
	src/HashMe.cpp
	src/Utils.cpp
	src/Base64.cpp
	src/SHA256.cpp
	src/SHA256_Hardware.cpp
	src/SHA224.cpp
	src/SHA224_Hardware.cpp
	src/SHA512.cpp
	src/SHA512_Hardware.cpp
	src/SHA384.cpp
	src/SHA384_Hardware.cpp
	src/MD5.cpp
	src/CRC_Software.cpp
	src/CRC32_Hardware.cpp
)

# The projects include directories
target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

###########################################################
# Project versioning
configure_file("cmake additional/HashMeVersion.hpp.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/include/HashMeVersion.hpp")

###########################################################
# Examples
add_subdirectory("examples/simple example")

#############################################################
# Optimization settings
if (WIN32)
	if (MSVC)
		target_compile_options("${PROJECT_NAME}" PRIVATE "/O2")
	endif()
else()
	target_compile_options("${PROJECT_NAME}" PRIVATE "-O3")
endif()

#############################################################
# SIMD specific compiler settings
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang") # GCC
	if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64") # Do we compile for arm64?
		target_compile_options("${PROJECT_NAME}" PRIVATE "-march=armv8-a+crypto+crc")
		#target_compile_options("${PROJECT_NAME}" PRIVATE "-ftree-vectorize")
	elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" OR CMAKE_SYSTEM_PROCESSOR STREQUAL "Win32")
		target_compile_options("${PROJECT_NAME}" PRIVATE "-march=native")
		target_compile_options("${PROJECT_NAME}" PRIVATE "-msse4.1")
		target_compile_options("${PROJECT_NAME}" PRIVATE "-msha")
		#target_compile_options("${PROJECT_NAME}" PRIVATE "-ftree-vectorize")
	endif()
endif()

###########################################################
# Macro for removing /RTC flag in VisualStudio (incompatible with /O2 optimization)
macro(RemoveRTCFlagDebug_VS)
	string(REGEX REPLACE "/RTC[^ ]*" "" CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG}")
endmacro()

macro(RemoveRTCFlagRelease_VS)
	string(REGEX REPLACE "/RTC[^ ]*" "" CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}")
endmacro()

###########################################################
# Add as many warnings as possible
if (WIN32)
	if (MSVC)
		RemoveRTCFlagDebug_VS()
		RemoveRTCFlagRelease_VS()

		target_compile_options("${PROJECT_NAME}" PRIVATE "/W3")
		#target_compile_options("${PROJECT_NAME}" PRIVATE "/WX")
		#target_compile_options("${PROJECT_NAME}" PRIVATE "/wd4244")
		#target_compile_options("${PROJECT_NAME}" PRIVATE "/wd4267")
		target_compile_options("${PROJECT_NAME}" PRIVATE "/D_CRT_SECURE_NO_WARNINGS")
	endif()
	# Force Win32 to UNICODE
	target_compile_definitions("${PROJECT_NAME}" PRIVATE UNICODE _UNICODE)
	#target_compile_options("${PROJECT_NAME}" PRIVATE "/utf-8")
else()
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wall")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wextra")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-pedantic")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wmissing-declarations")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wdeprecated")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wshadow")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wsign-conversion")
endif()

###########################################################
# Helper for formatting and printing text with color
include("cmake additional/colorFormatting.cmake")

###########################################################
# Include summary file for printing various information
include("cmake additional/printSummary.cmake")

# Print the information
_PRINT_SUMMARY()
