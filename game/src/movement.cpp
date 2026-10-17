#include "movement.h"

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

} // namespace game
