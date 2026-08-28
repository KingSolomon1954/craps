# -----------------------------------------------------------------
#
# CMakeLists file to compile library
#
# -----------------------------------------------------------------

add_library(cui STATIC
    src/CarrierBet.cpp
    src/ConsoleManager.cpp
    src/Cui.cpp
    src/CuiUtils.cpp
    src/DialogAckError.cpp
    src/DialogAmountEntry.cpp
    src/LayoutCrapsScreen.cpp
    src/MenuBetting.cpp
    src/MenuOddsBet.cpp
    src/MenuPlaceBet.cpp
    src/MenuPlayerViews.cpp
    src/ScreenCrapsTable.cpp
    src/WindowNavBar.cpp
    src/WindowPlayerArea.cpp
)

target_include_directories(cui
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(cui
    PUBLIC gen controller
    PRIVATE ncurses
)

target_compile_definitions(cui
    PRIVATE $<$<CONFIG:Debug>:
    POCO_LOG_DEBUG=1
    _DEBUG=1
    >
)

# -----------------------------------------------------------------
