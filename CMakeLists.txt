cmake_minimum_required(VERSION 3.29)

# Print build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

###########################################################
# Set options for sub projects
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # Needed to set options of subprojects inside parent project

###########################################################
# Use C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS false) # Don't use GNU++

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
	include/SHA256.hpp
	include/SHA256_Hardware.hpp
	include/MD5.hpp
	src/HashMe.cpp
	src/Utils.cpp
	src/SHA256.cpp
	src/SHA256_Hardware.cpp
	src/MD5.cpp
)

# The projects include directories
target_include_directories("${PROJECT_NAME}" PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

###########################################################
# Project versioning
configure_file(HashMeVersion.hpp.cmake "${CMAKE_CURRENT_SOURCE_DIR}/include/HashMeVersion.hpp")

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
else()
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wall")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wextra")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-pedantic")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wmissing-declarations")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wdeprecated")
	target_compile_options("${PROJECT_NAME}" PRIVATE "-Wshadow")
	#target_compile_options("${PROJECT_NAME}" PRIVATE "-Werror")
	#target_compile_options("${PROJECT_NAME}" PRIVATE "-Wsign-conversion")
endif()
