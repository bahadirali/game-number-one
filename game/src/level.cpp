#include "level.h"

#include <algorithm>

namespace game {

bool has_block(const Level& level, Cell cell) {
    return std::ranges::find(level.blocks, cell) != level.blocks.end();
}

std::optional<Level> parse_level(std::span<const std::string_view> rows) {
    if (rows.empty()) {
        return std::nullopt;
    }

    std::size_t width = 0;
    for (std::string_view row : rows) {
        width = std::max(width, row.size());
    }
    if (width == 0) {
        return std::nullopt;
    }

    Level level{Grid(static_cast<int>(width), static_cast<int>(rows.size()), Tile::Wall), {}};
    int player_count = 0;

    for (std::size_t y = 0; y < rows.size(); ++y) {
        for (std::size_t x = 0; x < rows[y].size(); ++x) {
            const Cell cell{static_cast<int>(x), static_cast<int>(y)};
            switch (rows[y][x]) {
                case '#':
                    level.grid.set(cell, Tile::Wall);
                    break;
                case '.':
                case ' ':
                    level.grid.set(cell, Tile::Floor);
                    break;
                case '$':
                    level.grid.set(cell, Tile::Floor);
                    level.blocks.push_back(cell);
                    break;
                case '@':
                    level.grid.set(cell, Tile::Floor);
                    level.player = cell;
                    ++player_count;
                    break;
                default:
                    return std::nullopt;
            }
        }
    }

    if (player_count != 1) {
        return std::nullopt;
    }
    return level;
}

} // namespace game
