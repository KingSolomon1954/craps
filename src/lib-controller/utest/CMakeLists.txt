# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(controller-ut
    utest-controller-main.cpp
    BetExecutorTest.cpp
    ConfigManagerTest.cpp
    ConfigSeedTest.cpp
    StartChoicesTest.cpp
    LastFillTest.cpp
    PlayerManagerNameTest.cpp
    PlayerManifestTest.cpp
    PlayerPrefsTest.cpp
    AutoOddsTest.cpp
    BetFlagsTest.cpp
    QuickBetsTest.cpp
    TableManifestTest.cpp
    UndoManagerTest.cpp
    UserStateTest.cpp
    YamlFileTest.cpp
)

target_link_libraries(controller-ut
    PRIVATE controller
    PRIVATE gen
    PRIVATE doctest::doctest
)

doctest_discover_tests(controller-ut)

# -----------------------------------------------------------------
