#include "engine/renderer.h"

#include <SDL3/SDL.h>

#include <string>

namespace engine {

void Renderer::clear(Color color) {
    SDL_SetRenderDrawColor(sdl_renderer_, color.r, color.g, color.b, color.a);
    SDL_RenderClear(sdl_renderer_);
}

void Renderer::fill_rect(const Rect& rect, Color color) {
    SDL_SetRenderDrawColor(sdl_renderer_, color.r, color.g, color.b, color.a);
    const SDL_FRect sdl_rect{rect.x, rect.y, rect.w, rect.h};
    SDL_RenderFillRect(sdl_renderer_, &sdl_rect);
}

void Renderer::draw_text(float x, float y, std::string_view text, Color color, float scale) {
    static_assert(kGlyphSize == SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE);

    // The font has a fixed pixel size, so magnify it with the render scale.
    // That scale also applies to the position, hence the division.
    float old_scale_x = 1.0f;
    float old_scale_y = 1.0f;
    SDL_GetRenderScale(sdl_renderer_, &old_scale_x, &old_scale_y);
    SDL_SetRenderScale(sdl_renderer_, scale, scale);

    SDL_SetRenderDrawColor(sdl_renderer_, color.r, color.g, color.b, color.a);
    const std::string terminated(text);  // SDL wants a null-terminated string
    SDL_RenderDebugText(sdl_renderer_, x / scale, y / scale, terminated.c_str());

    SDL_SetRenderScale(sdl_renderer_, old_scale_x, old_scale_y);
}

} // namespace engine
