#pragma once

#include "grid.h"

namespace game {

enum class Direction {
    Up,
    Down,
    Left,
    Right,
};

// The neighbouring cell one step in `direction` (y grows downwards).
Cell step(Cell from, Direction direction);

// Where the player ends up after trying to move one cell: the next cell if
// it's floor, otherwise `from` unchanged.
Cell try_move(const Grid& grid, Cell from, Direction direction);

} // namespace game
