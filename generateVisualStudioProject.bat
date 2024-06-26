@echo off

IF "%~1" == "" GOTO HELP_TEXT

set "BUILD_TYPE=%~1"
set "TARGET_IDE=%~2"

cmake -DCMAKE_BUILD_TYPE="%BUILD_TYPE%" -G "%TARGET_IDE%" -S . -B projects/ --fresh

exit

:HELP_TEXT
echo Usage:
echo.
echo generateVisualStudioProject.bat BUILD_TYPE VISUAL_STUDIO_VERSION
echo     BUILD_TYPE: Debug, Release
echo     VISUAL_STUDIO_VERSION: "Visual Studio 17 2022" or "Visual Studio 16 2019" or older...
