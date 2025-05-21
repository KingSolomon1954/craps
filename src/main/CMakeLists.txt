# --------------------------------------------------------
#
# CMakeLists file to compile RoyalCraps main
#
# --------------------------------------------------------

find_package(cxxopts REQUIRED)
find_package(rang REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(RoyalCraps
    src/main.cpp
)

target_include_directories(RoyalCraps
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(RoyalCraps
    PUBLIC ${PROJ_COMPILE_OPTIONS}
)

target_link_libraries(RoyalCraps
    PRIVATE gen
    PRIVATE craps
    PRIVATE controller
    PRIVATE cui
    PRIVATE gui
    PRIVATE cxxopts::cxxopts
    PRIVATE rang::rang
    PRIVATE nlohmann_json::nlohmann_json
)

# --------------------------------------------------------
