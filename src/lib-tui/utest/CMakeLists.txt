# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(tui-ut
    utest-tui-main.cpp
    PlaceholderTest.cpp
)

target_link_libraries(tui-ut
    PRIVATE tui
    PRIVATE doctest::doctest
)

doctest_discover_tests(tui-ut)

# -----------------------------------------------------------------
