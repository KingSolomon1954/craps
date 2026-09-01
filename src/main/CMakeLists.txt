# --------------------------------------------------------
#
# CMakeLists file to compile RoyalCraps main
#
# --------------------------------------------------------

add_executable(royalcraps
    src/main.cpp
)

target_include_directories(royalcraps
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(royalcraps
    PUBLIC ${PROJ_COMPILE_OPTIONS}
)

target_link_libraries(royalcraps
    PRIVATE gen
    PRIVATE craps
    PRIVATE controller
    PRIVATE cui
    PRIVATE gui
)

#------------------

add_executable(ncurses-snippet
    src/snippets/ncurses-snippet.cpp
)

target_link_libraries(ncurses-snippet
    PRIVATE ncursesw
)

#------------------

add_executable(ncurses-animation
    src/snippets/ncurses-animation.cpp
)

target_link_libraries(ncurses-animation
    PRIVATE ncursesw
)

#------------------

add_executable(ncurses-dynamic-menu
    src/snippets/ncurses-dynamic-menu.cpp
)

target_link_libraries(ncurses-dynamic-menu
    PRIVATE ncursesw
)

#------------------

add_executable(ncurses-too-small-resize
    src/snippets/ncurses-too-small-resize.cpp
)

target_link_libraries(ncurses-too-small-resize
    PRIVATE ncursesw
)

#------------------




# add_executable(dialog-amount-entry
#     src/snippets/dialog-amount-entry.cpp
# )

# target_link_libraries(dialog-amount-entry
#     PRIVATE ncursesw
#     PRIVATE cui
# )

# --------------------------------------------------------
