#pragma once

#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include "grid.h"

namespace game {

struct Level {
    Grid grid;
    Cell player;
    std::vector<Cell> blocks;  // pushable blocks, each on a floor cell
};

// Whether a block sits on `cell`.
bool has_block(const Level& level, Cell cell);

// Builds a level from rows of text:
//   '#' wall    '.' or ' ' floor    '@' player (on floor)    '$' block (on floor)
// Short rows are padded with walls. Returns nothing if the text has no rows,
// an unknown character, or not exactly one player.
std::optional<Level> parse_level(std::span<const std::string_view> rows);

} // namespace game
