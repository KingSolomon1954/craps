# --------------------------------------------------------
#
# CMakeLists file to compile lib-craps
#
# --------------------------------------------------------

find_package(nlohmann_json REQUIRED)
find_package(yaml-cpp REQUIRED)

add_library(craps STATIC
    src/Bank.cpp
    src/BankStats.cpp
    src/BetStats.cpp
    src/CrapsBet.cpp
    src/CrapsTable.cpp
    src/Dice.cpp
    src/EnumBetName.cpp
    src/Player.cpp
    src/RollStats.cpp
    src/TableStats.cpp
)

target_include_directories(craps
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(craps
    PUBLIC gen
    PUBLIC controller
    PUBLIC nlohmann_json::nlohmann_json
    PUBLIC yaml-cpp::yaml-cpp
)

target_compile_definitions(craps
    PRIVATE $<$<CONFIG:Debug>:
    POCO_LOG_DEBUG=1
    _DEBUG=1
    >
)

# --------------------------------------------------------
