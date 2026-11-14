# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(${APP_EXEC}-ut
    utest-main.cpp
)

target_link_libraries(${APP_EXEC}-ut
    PRIVATE doctest::doctest
)

doctest_discover_tests(${APP_EXEC}-ut)

# -----------------------------------------------------------------
