# CMake fragment to obtain APP_EXEC from top-level Makefile

if (NOT PROJECT_TOP)
    message(FATAL_ERROR "PROJECT_TOP not found. Aborting...")
endif()

set(_makefile "${PROJECT_TOP}/Makefile")

file(STRINGS "${_makefile}" _app_exec_line
     REGEX "^[ \t]*APP_EXEC[ \t]*[:?+]?=")

if(NOT _app_exec_line)
    message(FATAL_ERROR "APP_EXEC not found in ${_makefile}")
endif()

string(REGEX REPLACE
       "^[ \t]*APP_EXEC[ \t]*[:?+]?=[ \t]*([^ \t#]+).*"
       "\\1"
       APP_EXEC
       "${_app_exec_line}")

# message(STATUS "APP_EXEC=${APP_EXEC}")
