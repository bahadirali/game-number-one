# game_logic: the rules of the game, with no window, input or drawing.
# Kept separate so the tests can link it without SDL or the engine.
add_library(game_logic STATIC
    src/ai.cpp
    src/level.cpp
    src/level_file.cpp
    src/match.cpp
    src/movement.cpp
)
target_include_directories(game_logic PUBLIC src)
target_link_libraries(game_logic PRIVATE project_options)

# game: the executable that plugs the game logic into the engine.
add_executable(game
    src/main.cpp
    src/duel_game.cpp
    src/puzzle_game.cpp
)
target_link_libraries(game PRIVATE game_logic engine::engine project_options)

# Release builds open no console window. We keep the standard main() entry
# point on MSVC rather than switching to WinMain.
set_target_properties(game PROPERTIES WIN32_EXECUTABLE $<NOT:$<CONFIG:Debug>>)
if(MSVC)
    target_link_options(game PRIVATE /ENTRY:mainCRTStartup)
endif()

# Level files ship in a levels/ folder next to the executable. The copy runs
# on every build but only touches changed files, so editing a level just
# needs a rebuild.
add_custom_target(copy_levels ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${PROJECT_SOURCE_DIR}/levels" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/levels"
    COMMENT "Copying levels next to the executable")
add_dependencies(game copy_levels)

game_copy_asan_runtime(game)

set_property(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT game)
