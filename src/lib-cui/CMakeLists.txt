# -----------------------------------------------------------------
#
# CMakeLists file to compile library
#
# -----------------------------------------------------------------

add_library(cui STATIC
    src/CarrierBet.cpp
    src/CuiMain.cpp
    src/CuiThread.cpp
    src/CuiUtils.cpp
    src/CuiView.cpp
    src/SurfaceManager.cpp
#   src/dialogs/DialogAckError.cpp
#   src/dialogs/DialogAmountEntry.cpp
    src/layouts/LayoutCrapsScreen.cpp
    src/menus/MenuBetting.cpp
    src/menus/MenuOddsBet.cpp
    src/menus/MenuPlaceBet.cpp
    src/menus/MenuNavBarViews.cpp
    src/screens/ScreenCrapsTable.cpp
    src/panels/WindowNavBar.cpp
    src/panels/WindowPlayerArea.cpp
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
