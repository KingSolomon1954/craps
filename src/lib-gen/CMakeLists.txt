# -----------------------------------------------------------------
#
# CMakeLists file to compile library
#
# -----------------------------------------------------------------

find_package(Boost REQUIRED COMPONENTS asio)
find_package(rang REQUIRED)
find_package(yaml-cpp REQUIRED)

add_library(gen STATIC
    src/BuildInfo.cpp
    src/ErrorPass.cpp
    src/Logger.cpp
    src/Money.cpp
    src/MoneyUtil.cpp
    src/MultiLayerConfig.cpp
    src/StringUtils.cpp
    src/TimerManager.cpp
    src/Timepoint.cpp
    src/Uuid.cpp
)

# Some files are auto-generated in the _build folder, for example
# BuildInfoDefs.h, so we need to add binary folder to include path
target_include_directories(gen
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_libraries(gen
    PRIVATE boost::boost
    PUBLIC rang::rang
    PUBLIC yaml-cpp::yaml-cpp
)

target_compile_definitions(gen
    PRIVATE $<$<CONFIG:Debug>:
    POCO_LOG_DEBUG=1
    _DEBUG=1
    >
)

include(buildinfodefs)
  
# -----------------------------------------------------------------
