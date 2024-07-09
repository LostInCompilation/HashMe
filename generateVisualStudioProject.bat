::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Script for generating Visual Studio project files.                         ::
::                                                                            ::
:: (C) 2024 Marc Schöndorf                                                    ::
:: Licensed under the zlib License. See LICENSE.md                            ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

@echo off

IF "%~1" == "" GOTO HELP_TEXT
IF "%~2" == "" GOTO HELP_TEXT
IF "%~3" == "" GOTO HELP_TEXT

set "BUILD_TYPE=%~1"
set "TARGET_IDE=%~2"
set "ARCHITECTURE=%~3"

cmake -DCMAKE_CONFIGURATION_TYPES="%BUILD_TYPE%" -G "%TARGET_IDE%" -A "%ARCHITECTURE%" -S . -B projects/ --fresh

exit

:HELP_TEXT
echo.
echo Usage:
echo.
echo generateVisualStudioProject.bat BUILD_TYPE VISUAL_STUDIO_VERSION ARCHITECTURE
echo.
echo     BUILD_TYPE: Debug, Release, RelWithDebInfo
echo     VISUAL_STUDIO_VERSION: "Visual Studio 17 2022", "Visual Studio 16 2019" or older...
echo     ARCHITECTURE: Win32, x64, ARM, ARM64
