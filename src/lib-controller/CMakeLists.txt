# -----------------------------------------------------------------
#
# CMakeLists file to compile library
#
# -----------------------------------------------------------------

find_package(cxxopts REQUIRED)
find_package(rang REQUIRED)
find_package(yaml-cpp REQUIRED)

add_library(controller STATIC
    src/ConfigManager.cpp
    src/ConfigCommandLine.cpp
    src/ConfigDefaults.cpp
    src/ConfigEnv.cpp
    src/ConfigFiles.cpp
    src/ConfigSeed.cpp
    src/StartChoices.cpp
    src/BetExecutor.cpp
    src/BetSpec.cpp
    src/CrapsReaders.cpp
    src/EventHandler.cpp
    src/GameEvents.cpp
    src/GameMain.cpp
    src/Globals.cpp
    src/PlayerManager.cpp
    src/PlayerManifest.cpp
    src/PlayerPrefs.cpp
    src/QuickBets.cpp
    src/Properties.cpp
    src/LastFill.cpp
    src/TableManager.cpp
    src/TableManifest.cpp
    src/UndoBet.cpp
    src/UndoManager.cpp
    src/UserState.cpp
    src/YamlFile.cpp
)

target_include_directories(controller
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(controller
    PUBLIC gen
    PUBLIC craps
    PUBLIC cui
    PUBLIC gui
    PRIVATE cxxopts::cxxopts
    PRIVATE rang::rang
    PRIVATE yaml-cpp::yaml-cpp
)

target_compile_definitions(controller
    PRIVATE $<$<CONFIG:Debug>:
    POCO_LOG_DEBUG=1
    _DEBUG=1
    >
)

# -----------------------------------------------------------------
