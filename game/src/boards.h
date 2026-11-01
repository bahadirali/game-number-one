#pragma once

#include <array>
#include <string_view>

namespace game::boards {

// The one-player puzzle room, still hard-coded. Duel levels live in the
// levels/ folder instead (see level_file.h).
inline constexpr std::array<std::string_view, 9> kPuzzleRows = {
    "############",
    "#..........#",
    "#..###..$..#",
    "#....#..#..#",
    "#.@$.#..#..#",
    "#....$..##.#",
    "#..##...$..#",
    "#..........#",
    "############",
};

} // namespace game::boards
