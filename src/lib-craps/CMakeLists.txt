# --------------------------------------------------------
#
# CMakeLists file to compile lib-craps
#
# --------------------------------------------------------

find_package(nlohmann_json REQUIRED)

add_library(craps STATIC
  
#    src/Bank.cpp
#    src/CrapsBet.cpp
#    src/CrapsGame.cpp
#    src/CrapsTable.cpp
    src/Dice.cpp
#    src/EnumBetName.cpp
#    src/Globals.cpp
#    src/Player.cpp
#    src/PlayerManager.cpp
#    src/PlayerPicker.cpp
#    src/Properties.cpp
#    src/TablePicker.cpp
)

target_include_directories(craps
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(craps
    PUBLIC gen
    PUBLIC nlohmann_json::nlohmann_json
)

target_compile_definitions(craps
    PRIVATE $<$<CONFIG:Debug>:
    POCO_LOG_DEBUG=1
    _DEBUG=1
    >
)

# --------------------------------------------------------
