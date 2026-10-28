#pragma once

#include "grid.h"
#include "level.h"

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

// Moves the player one cell. Walking into a block pushes it one cell further,
// but only if that cell is floor with no other block on it: blocks never push
// other blocks. Returns whether the player moved.
bool move_player(Level& level, Direction direction);

} // namespace game
