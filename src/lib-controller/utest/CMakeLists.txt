# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(controller-ut
    utest-controller-main.cpp
    PlaceholderTest.cpp
)

target_link_libraries(controller-ut
    PRIVATE controller
    PRIVATE gen
    PRIVATE doctest::doctest
)

doctest_discover_tests(controller-ut)

# -----------------------------------------------------------------
