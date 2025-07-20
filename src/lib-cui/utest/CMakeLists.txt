# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(cui-ut
    utest-cui-main.cpp
    PlaceholderTest.cpp
)

target_link_libraries(cui-ut
    PRIVATE cui
    PRIVATE doctest::doctest
)

doctest_discover_tests(cui-ut)

# -----------------------------------------------------------------
