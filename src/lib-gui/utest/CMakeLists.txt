# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(gui-ut
    utest-gui-main.cpp
    PlaceholderTest.cpp
)

target_link_libraries(gui-ut
    PRIVATE gui
    PRIVATE doctest::doctest
)

doctest_discover_tests(gui-ut)

# -----------------------------------------------------------------
