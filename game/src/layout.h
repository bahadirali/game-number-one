#pragma once

#include <engine/renderer.h>

#include "grid.h"

namespace game {

constexpr int kCellSize = 48;  // pixels per grid cell

// The screen rectangle of a grid cell, shrunk by `inset` pixels on each side.
inline engine::Rect cell_rect(Cell cell, float inset = 0.0f) {
    return {
        static_cast<float>(cell.x * kCellSize) + inset,
        static_cast<float>(cell.y * kCellSize) + inset,
        static_cast<float>(kCellSize) - 2.0f * inset,
        static_cast<float>(kCellSize) - 2.0f * inset,
    };
}

} // namespace game
