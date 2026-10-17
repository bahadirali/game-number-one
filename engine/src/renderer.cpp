#include "engine/renderer.h"

#include <SDL3/SDL.h>

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

} // namespace engine
