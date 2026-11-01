add_executable(tests
    main.cpp
    movement_tests.cpp
    push_tests.cpp
    match_tests.cpp
    ai_tests.cpp
    boards_tests.cpp
    level_file_tests.cpp
    level_tests.cpp
    input_tests.cpp
    app_tests.cpp
)
target_link_libraries(tests PRIVATE game_logic engine::engine doctest::doctest project_options)
# Lets the tests read the real level files straight from the source tree.
target_compile_definitions(tests PRIVATE GAME_LEVELS_DIR="${PROJECT_SOURCE_DIR}/levels/")

# Must come before doctest_discover_tests, which runs the exe after building.
game_copy_asan_runtime(tests)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(tests)
