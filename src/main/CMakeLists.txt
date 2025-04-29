# --------------------------------------------------------
#
# CMakeLists file to compile RoyalCraps
#
# --------------------------------------------------------

find_package(cxxopts REQUIRED)
find_package(rang REQUIRED)

add_library(main STATIC
    src/CommandLine.cpp
    src/CrapsGame.cpp
    src/CrapsBet.cpp
    src/CrapsTable.cpp
    src/Dice.cpp
    src/EnumBetName.cpp
    src/Globals.cpp
    src/Player.cpp
    src/PlayerManager.cpp
    src/PlayerIdGenerator.cpp
    src/Properties.cpp
)

target_include_directories(main
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(main
    PUBLIC gen
    PUBLIC codec
    PUBLIC cxxopts::cxxopts
    PUBLIC rang::rang
)

target_compile_options(main
    PUBLIC ${PROJ_COMPILE_OPTIONS}
)

target_compile_definitions(main
    PRIVATE $<$<CONFIG:Debug>:
    POCO_LOG_DEBUG=1
    _DEBUG=1
    >
)

add_executable(RoyalCraps
    src/main.cpp
)

target_link_libraries(RoyalCraps
    PRIVATE main
)

# --------------------------------------------------------
