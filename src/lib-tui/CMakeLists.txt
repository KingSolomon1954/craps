# -----------------------------------------------------------------
#
# CMakeLists file to compile library
#
# -----------------------------------------------------------------

add_library(tui STATIC
    src/Placeholder.cpp
)

target_include_directories(tui
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(tui
    PUBLIC gen controller
)

target_compile_definitions(tui
    PRIVATE $<$<CONFIG:Debug>:
    POCO_LOG_DEBUG=1
    _DEBUG=1
    >
)

# -----------------------------------------------------------------
