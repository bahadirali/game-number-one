#include "engine/app.h"

#include <SDL3/SDL.h>
// The game keeps its own plain main(); this only declares SDL_SetMainReady().
#define SDL_MAIN_HANDLED
#include <SDL3/SDL_main.h>

#include <optional>

namespace engine {

namespace {

std::optional<Key> to_key(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_UP: return Key::Up;
        case SDL_SCANCODE_DOWN: return Key::Down;
        case SDL_SCANCODE_LEFT: return Key::Left;
        case SDL_SCANCODE_RIGHT: return Key::Right;
        case SDL_SCANCODE_W: return Key::W;
        case SDL_SCANCODE_A: return Key::A;
        case SDL_SCANCODE_S: return Key::S;
        case SDL_SCANCODE_D: return Key::D;
        case SDL_SCANCODE_SPACE: return Key::Space;
        case SDL_SCANCODE_R: return Key::R;
        case SDL_SCANCODE_ESCAPE: return Key::Escape;
        default: return std::nullopt;
    }
}

// Returns false when the user asked to close the window.
bool pump_events(Input& input) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_EVENT_QUIT:
                return false;
            case SDL_EVENT_KEY_DOWN:
                // The engine does its own key repeat, so ignore the OS's.
                if (!event.key.repeat) {
                    if (auto key = to_key(event.key.scancode)) {
                        input.on_key_down(*key);
                    }
                }
                break;
            case SDL_EVENT_KEY_UP:
                if (auto key = to_key(event.key.scancode)) {
                    input.on_key_up(*key);
                }
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                // Key-up events are lost while unfocused; don't leave keys stuck.
                input.release_all();
                break;
            default:
                break;
        }
    }
    return true;
}

} // namespace

int run(const AppConfig& config, Game& game) {
    // Zero would never update; a negative rate would make the tick length
    // negative and trap the update loop forever.
    if (config.updates_per_second <= 0) {
        SDL_Log("AppConfig::updates_per_second must be positive, got %d",
                config.updates_per_second);
        return 1;
    }

    SDL_SetMainReady();

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* sdl_renderer = nullptr;
    const SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    if (!SDL_CreateWindowAndRenderer(config.title.c_str(), config.width, config.height, flags,
                                     &window, &sdl_renderer)) {
        SDL_Log("Creating the window failed: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Draw at a fixed logical size and let SDL scale it to the window.
    SDL_SetRenderLogicalPresentation(sdl_renderer, config.width, config.height,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    const bool vsync = SDL_SetRenderVSync(sdl_renderer, 1);

    Input input;
    Renderer renderer(sdl_renderer);
    game.setup(input);

    // Fixed timestep: update exactly `updates_per_second` times per second of
    // real time, however fast or slow frames are drawn.
    const double tick_seconds = 1.0 / config.updates_per_second;
    const double counter_frequency = static_cast<double>(SDL_GetPerformanceFrequency());
    constexpr double kMaxFrameSeconds = 0.25;  // avoid a burst of updates after a stall

    Uint64 previous = SDL_GetPerformanceCounter();
    double accumulator = 0.0;
    bool running = true;

    while (running) {
        running = pump_events(input);

        const Uint64 now = SDL_GetPerformanceCounter();
        double frame_seconds = static_cast<double>(now - previous) / counter_frequency;
        previous = now;
        if (frame_seconds > kMaxFrameSeconds) {
            frame_seconds = kMaxFrameSeconds;
        }
        accumulator += frame_seconds;

        while (running && accumulator >= tick_seconds) {
            input.tick();
            game.update(input);
            accumulator -= tick_seconds;
            if (game.wants_quit()) {
                running = false;
            }
        }

        renderer.clear(config.clear_color);
        game.render(renderer);
        SDL_RenderPresent(sdl_renderer);

        if (!vsync) {
            SDL_Delay(1);  // don't spin a CPU core at 100%
        }
    }

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

} // namespace engine
