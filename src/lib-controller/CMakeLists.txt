# -----------------------------------------------------------------
#
# CMakeLists file to compile library
#
# -----------------------------------------------------------------

add_library(controller STATIC
    src/Placeholder.cpp
    src/AppController.cpp
)

target_include_directories(controller
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(controller
    PUBLIC gen
    PUBLIC craps
)

target_compile_definitions(controller
    PRIVATE $<$<CONFIG:Debug>:
    POCO_LOG_DEBUG=1
    _DEBUG=1
    >
)

# -----------------------------------------------------------------
