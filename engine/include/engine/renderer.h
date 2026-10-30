#pragma once

#include <cstdint>
#include <string_view>

struct SDL_Renderer;

namespace engine {

struct Color {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;
};

// A rectangle in logical pixels: (0, 0) is the window's top-left corner.
struct Rect {
    float x = 0.0f;
    float y = 0.0f;
    float w = 0.0f;
    float h = 0.0f;
};

// Thin drawing interface handed to Game::render(). Coordinates are in the
// logical resolution from AppConfig, independent of window size or DPI.
class Renderer {
public:
    explicit Renderer(SDL_Renderer* sdl_renderer) : sdl_renderer_(sdl_renderer) {}

    void clear(Color color);
    void fill_rect(const Rect& rect, Color color);

    // Draws `text` in SDL's small built-in pixel font, magnified `scale`
    // times, with its top-left corner at (x, y). ASCII only, and blocky, but
    // it needs no font file: good enough for short status messages.
    void draw_text(float x, float y, std::string_view text, Color color, float scale = 1.0f);

    // The size draw_text covers. Every character is the same square size.
    static float text_width(std::string_view text, float scale = 1.0f) {
        return static_cast<float>(text.size()) * text_height(scale);
    }
    static float text_height(float scale = 1.0f) { return kGlyphSize * scale; }

private:
    static constexpr float kGlyphSize = 8.0f;  // SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE

    SDL_Renderer* sdl_renderer_;
};

} // namespace engine
