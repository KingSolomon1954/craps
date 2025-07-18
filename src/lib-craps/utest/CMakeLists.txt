# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(craps-ut
    utest-lib-craps.cpp
    CrapsBetPart1Test.cpp
    CrapsBetPart2Test.cpp
    CrapsTableTest.cpp
    EnumBetNameTest.cpp
    SessionHistoryTest.cpp
    TableStatsPart1Test.cpp
    TableStatsPart2Test.cpp
    TableStatsPart3Test.cpp
    TableStatsPart4Test.cpp
    TableStatsPart5Test.cpp
)

target_link_libraries(craps-ut
    PRIVATE craps
    PRIVATE gen
    PRIVATE doctest::doctest
)

doctest_discover_tests(craps-ut)

# -----------------------------------------------------------------
