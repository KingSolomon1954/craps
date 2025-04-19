# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(RoyalCraps-ut
    utest-main.cpp
    CrapsBetPart1Test.cpp
    CrapsBetPart2Test.cpp
    CrapsGameTest.cpp
    EnumBetNameTest.cpp
    PropertiesTest.cpp
)

target_link_libraries(RoyalCraps-ut
    PRIVATE main
    PRIVATE doctest::doctest
)

doctest_discover_tests(RoyalCraps-ut)

# -----------------------------------------------------------------
