cmake_minimum_required(VERSION 3.28)

project(GameNumberOne
    VERSION 0.1.0
    DESCRIPTION "A small grid puzzle game and the engine that runs it"
    LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Put every executable in one folder so they're easy to find and run.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

option(GAME_ENABLE_ASAN "Build Debug with AddressSanitizer" ON)
option(GAME_BUILD_TESTS "Build the unit tests" ON)

include(cmake/ProjectOptions.cmake)
include(cmake/Dependencies.cmake)

add_subdirectory(engine)
add_subdirectory(game)

if(GAME_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
