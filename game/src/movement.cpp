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

bool move_and_push(const Grid& grid, std::vector<Cell>& blocks, Cell& mover,
                   Direction direction, std::span<const Cell> occupied) {
    const auto is_occupied = [&](Cell cell) {
        return std::ranges::find(occupied, cell) != occupied.end();
    };

    const Cell to = try_move(grid, mover, direction);
    if (to == mover || is_occupied(to)) {
        return false;
    }

    const auto block = std::ranges::find(blocks, to);
    if (block != blocks.end()) {
        const Cell pushed_to = step(to, direction);
        if (grid.at(pushed_to) != Tile::Floor || is_occupied(pushed_to) ||
            std::ranges::find(blocks, pushed_to) != blocks.end()) {
            return false;
        }
        *block = pushed_to;
    }

    mover = to;
    return true;
}

bool move_player(Level& level, Direction direction) {
    return move_and_push(level.grid, level.blocks, level.player, direction);
}

} // namespace game
