################################################################################
#              Print summary of some CMake variables and configs               #
#                                                                              #
# (C) 2024 Marc Schöndorf                                                      #
# Licensed under the zlib License. See LICENSE.md                              #
################################################################################

macro(_PRINT_SUMMARY)

message("\n\n\n\n\n\n")

colorFormatTextAppend(COLOR RED "This is ")
colorFormatTextAppend(COLOR GREEN "a multicolor ")
colorFormatTextAppend(BOLD COLOR BLUE "message ")
colorFormatTextAppend(COLOR MAGENTA "test")
message("${COLOR_FORMATTED_TEXT_COMBINED}")

message("\n\n\n\n\n\n\n\n\n\n\n\n")



message("################################################################################
#                                   General                                    #
################################################################################")
colorFormatText(BOLD COLOR BLUE "Build type:")
message("${COLOR_FORMATTED_TEXT} ${CMAKE_BUILD_TYPE}")
colorFormatText(BOLD COLOR BLUE "Compiler:")
message("${COLOR_FORMATTED_TEXT}   ${CMAKE_CXX_COMPILER_ID}\n")

message("################################################################################
#                                   Platform                                   #
################################################################################")
colorFormatText(BOLD COLOR BLUE "Host System Processor:")
message("${COLOR_FORMATTED_TEXT}   ${CMAKE_HOST_SYSTEM_PROCESSOR}")
colorFormatText(BOLD COLOR BLUE "Target System Processor:")
message("${COLOR_FORMATTED_TEXT} ${CMAKE_SYSTEM_PROCESSOR}\n")

colorFormatText(BOLD COLOR BLUE "System name:")
message("${COLOR_FORMATTED_TEXT} ${CMAKE_SYSTEM_NAME}")
colorFormatText(BOLD COLOR BLUE "UNIX:")
message("${COLOR_FORMATTED_TEXT}  ${UNIX}")
colorFormatText(BOLD COLOR BLUE "APPLE:")
message("${COLOR_FORMATTED_TEXT} ${APPLE}")
colorFormatText(BOLD COLOR BLUE "WIN32:")
message("${COLOR_FORMATTED_TEXT} ${WIN32}\n")

message("################################################################################
#                              Output directories                              #
#                                                                              #
#                                   Library                                    #
################################################################################")
colorFormatText(BOLD COLOR BLUE "CMAKE_LIBRARY_OUTPUT_DIRECTORY:")
message("${COLOR_FORMATTED_TEXT}         ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
colorFormatText(BOLD COLOR BLUE "CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG:")
message("${COLOR_FORMATTED_TEXT}   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG}")
colorFormatText(BOLD COLOR BLUE "CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE:")
message("${COLOR_FORMATTED_TEXT} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE}\n")

message("################################################################################
#                              Runtime executable                              #
################################################################################")
colorFormatText(BOLD COLOR BLUE "CMAKE_RUNTIME_OUTPUT_DIRECTORY:")
message("${COLOR_FORMATTED_TEXT}         ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
colorFormatText(BOLD COLOR BLUE "CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG:")
message("${COLOR_FORMATTED_TEXT}   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")
colorFormatText(BOLD COLOR BLUE "CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE:")
message("${COLOR_FORMATTED_TEXT} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}\n")

message("################################################################################
#                                Archive files                                #
################################################################################")
colorFormatText(BOLD COLOR BLUE "CMAKE_ARCHIVE_OUTPUT_DIRECTORY:")
message("${COLOR_FORMATTED_TEXT}         ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
colorFormatText(BOLD COLOR BLUE "CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:")
message("${COLOR_FORMATTED_TEXT}   ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG}")
colorFormatText(BOLD COLOR BLUE "CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:")
message("${COLOR_FORMATTED_TEXT} ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE}\n")

endmacro()
