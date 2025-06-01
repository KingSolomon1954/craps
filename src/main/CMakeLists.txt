# --------------------------------------------------------
#
# CMakeLists file to compile RoyalCraps main
#
# --------------------------------------------------------

add_executable(royalcraps
    src/main.cpp
)

target_include_directories(royalcraps
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(royalcraps
    PUBLIC ${PROJ_COMPILE_OPTIONS}
)

target_link_libraries(royalcraps
    PRIVATE gen
    PRIVATE craps
    PRIVATE controller
    PRIVATE cui
    PRIVATE gui
)

# --------------------------------------------------------
