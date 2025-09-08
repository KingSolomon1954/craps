# -----------------------------------------------------------------
#
# CMakeLists file to compile library
#
# -----------------------------------------------------------------

add_library(cui STATIC
    src/ConsoleView.cpp
    src/MenuBase.cpp
    src/MenuBetting.cpp
    src/ScreenCrapsTable.cpp
    src/ScreenFactory.cpp
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
