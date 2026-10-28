#include "movement.h"

#include <algorithm>

namespace game {

Cell step(Cell from, Direction direction) {
    switch (direction) {
        case Direction::Up: return {from.x, from.y - 1};
        case Direction::Down: return {from.x, from.y + 1};
        case Direction::Left: return {from.x - 1, from.y};
        case Direction::Right: return {from.x + 1, from.y};
    }
    return from;
}

Cell try_move(const Grid& grid, Cell from, Direction direction) {
    const Cell to = step(from, direction);
    return grid.at(to) == Tile::Floor ? to : from;
}

bool move_player(Level& level, Direction direction) {
    const Cell to = try_move(level.grid, level.player, direction);
    if (to == level.player) {
        return false;
    }

    const auto block = std::ranges::find(level.blocks, to);
    if (block != level.blocks.end()) {
        const Cell pushed_to = step(to, direction);
        if (level.grid.at(pushed_to) != Tile::Floor || has_block(level, pushed_to)) {
            return false;
        }
        *block = pushed_to;
    }

    level.player = to;
    return true;
}

} // namespace game
