# -----------------------------------------------------------------
#
# CMakeLists file to compile library
#
# -----------------------------------------------------------------

find_package(cxxopts REQUIRED)
find_package(rang REQUIRED)
find_package(yaml-cpp REQUIRED)

add_library(controller STATIC
    src/AutoFill.cpp
    src/ConfigManager.cpp
    src/ConfigCommandLine.cpp
    src/ConfigDefaults.cpp
    src/ConfigEnv.cpp
    src/ConfigFiles.cpp
    src/CrapsGame.cpp
    src/CrapsEventEmitters.cpp
    src/CrapsEventHandlers.cpp
    src/CrapsInterfaceReader.cpp
    src/EventLoop.cpp
    src/GameController.cpp
    src/Globals.cpp
    src/PlayerManager.cpp
    src/PlayerManifest.cpp
    src/Properties.cpp
    src/QuickBet.cpp
    src/TableManager.cpp
    src/TableManifest.cpp
    src/UndoBet.cpp
    src/UndoManager.cpp
    src/ViewEventEmitters.cpp
    src/ViewEventHandlers.cpp
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
