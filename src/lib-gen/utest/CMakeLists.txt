# -----------------------------------------------------------------
#
# CMakeLists file to compile unit tests
#
# -----------------------------------------------------------------

find_package(Boost REQUIRED COMPONENTS asio)

add_executable(gen-ut
    utest-gen-main.cpp
    BuildInfoTest.cpp
    MoneyTest.cpp
    MultiLayerConfigTest.cpp
    StringUtilsTest.cpp
    TimerManagerTest.cpp
)

target_link_libraries(gen-ut
    PRIVATE gen
    PRIVATE boost::boost
    PRIVATE doctest::doctest
)

doctest_discover_tests(gen-ut)

# -----------------------------------------------------------------
