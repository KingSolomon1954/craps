# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

add_executable(controller-ut
    utest-controller-main.cpp
    ConfigManagerTest.cpp
    ConfigSeedTest.cpp
    StartChoicesTest.cpp
    LastFillTest.cpp
    PlayerManagerNameTest.cpp
    PlayerManifestTest.cpp
    PlayerPrefsTest.cpp
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
