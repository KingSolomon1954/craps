# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(royalcraps-ut
    utest-main.cpp
)

target_link_libraries(royalcraps-ut
    PRIVATE doctest::doctest
)

doctest_discover_tests(royalcraps-ut)

# -----------------------------------------------------------------
