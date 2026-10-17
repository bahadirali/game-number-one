# The engine: a static library with no knowledge of any particular game.
# SDL3 is a PRIVATE dependency, so games only see the engine's own headers.

add_library(engine STATIC
    src/app.cpp
    src/input.cpp
    src/renderer.cpp
)
add_library(engine::engine ALIAS engine)

target_include_directories(engine PUBLIC include)
target_link_libraries(engine
    PRIVATE
        SDL3::SDL3-static
        project_options
)
