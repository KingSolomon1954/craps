# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(RoyalCraps-ut
    utest-main.cpp
    CrapsTest.cpp
    EnumBetNameTest.cpp
    PropertiesTest.cpp
)

target_link_libraries(RoyalCraps-ut
    PRIVATE main
    PRIVATE doctest::doctest
)

doctest_discover_tests(RoyalCraps-ut)

# -----------------------------------------------------------------
