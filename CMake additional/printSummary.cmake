################################################################################
#              Print summary of some CMake variables and configs               #
################################################################################

macro(_PRINT_SUMMARY)

message("################################################################################
#                                  Build type                                  #
################################################################################")
colorFormatText(BOLD COLOR BLUE "Build type:")
message("${COLOR_FORMATTED_TEXT} ${CMAKE_BUILD_TYPE}\n")

message("################################################################################
#                                   Platform                                   #
################################################################################")
colorFormatText(BOLD COLOR BLUE "System name:")
message("${COLOR_FORMATTED_TEXT} ${CMAKE_SYSTEM_NAME}")
colorFormatText(BOLD COLOR BLUE "UNIX:")
message("${COLOR_FORMATTED_TEXT} ${UNIX}")
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
