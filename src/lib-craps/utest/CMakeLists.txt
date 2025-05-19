# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(craps-ut
    utest-lib-craps.cpp

#    CrapsBetPart1Test.cpp
#    CrapsBetPart2Test.cpp
#    CrapsGameTest.cpp
#    CrapsTableTest.cpp
#    EnumBetNameTest.cpp
#    EventManagerTest.cpp
#    PropertiesTest.cpp
)

target_link_libraries(craps-ut
    PRIVATE gen
    PRIVATE doctest::doctest
)

doctest_discover_tests(craps-ut)

# -----------------------------------------------------------------
