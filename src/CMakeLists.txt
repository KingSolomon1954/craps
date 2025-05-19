# --------------------------------------------------------
#
# File: CMakeLists.txt
#
# Copyright (c) 2024 KingSolomon1954
#
# SPDX-License-Identifier: MIT
#
# --------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

project("RoyalCraps"
    DESCRIPTION "Multiplayer dice game"
)
  
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(PROJ_COMPILE_OPTIONS
    -Wall -Wextra -Werror -Werror=format-security
    -fstack-protector-strong
    -O3
)

set(PROJECT_TOP ${PROJECT_SOURCE_DIR}/..)

list(APPEND CMAKE_MODULE_PATH
    ${PROJECT_TOP}/tools/cmake
)

include(gen)

add_subdirectory(lib-gen)
add_subdirectory(lib-gen/utest)
add_subdirectory(lib-craps)
add_subdirectory(lib-craps/utest)
add_subdirectory(lib-controller)
add_subdirectory(lib-controller/utest)
add_subdirectory(lib-tui)
add_subdirectory(lib-tui/utest)
add_subdirectory(lib-gui)
add_subdirectory(lib-gui/utest)
add_subdirectory(main)
add_subdirectory(main/utest)

# -----------------------------------------------------------------
