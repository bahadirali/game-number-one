#pragma once

#include <string>

#include "engine/input.h"
#include "engine/renderer.h"

namespace engine {

struct AppConfig {
    std::string title = "Game";
    int width = 640;   // logical resolution; the window opens at this size
    int height = 480;
    int updates_per_second = 60;
    Color clear_color{};
};

// What a game implements. The engine owns the window and calls these.
class Game {
public:
    virtual ~Game() = default;

    // Called once after the window opens. Bind input actions here.
    virtual void setup(Input& input) { (void)input; }
    // Called at a fixed rate (AppConfig::updates_per_second).
    virtual void update(const Input& input) = 0;
    // Called once per frame, after the screen is cleared.
    virtual void render(Renderer& renderer) = 0;
    // Checked after every update; return true to close the app.
    virtual bool wants_quit() const { return false; }
};

// Opens the window, runs the fixed-timestep loop until the game quits or the
// window closes, then shuts everything down. Returns a process exit code.
int run(const AppConfig& config, Game& game);

} // namespace engine
