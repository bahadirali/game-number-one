#pragma once

#include <array>
#include <string_view>

namespace game::boards {

// Hard-coded boards until levels are loaded from files.
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

// Point-symmetric, so neither player starts with an edge. The players start
// on opposite sides with a dense field of blocks between them, so there's
// something to fight over from the first moves.
inline constexpr std::array<std::string_view, 13> kDuelRows = {
    "#################",
    "#...............#",
    "#...............#",
    "#....$.$.$.$....#",
    "#...$.$...$.$...#",
    "#.....$.#.$.....#",
    "#1...$.$.$.$...2#",
    "#.....$.#.$.....#",
    "#...$.$...$.$...#",
    "#....$.$.$.$....#",
    "#...............#",
    "#...............#",
    "#################",
};
inline constexpr int kDuelMoves = 50;  // 25 each

// A tiny corridor for checking the automatic pass by hand. Player 1's only
// move is left (player 2 is in the way on the right). Player 2's only move
// is then left too, into the cell player 1 just left, which shuts player 1
// in: their turn is passed for them. Press R for a round where player 2
// starts, and is stuck from the very first turn.
inline constexpr std::array<std::string_view, 3> kStuckTestRows = {
    "#################",
    "#######.12#######",
    "#################",
};
inline constexpr int kStuckTestMoves = 10;

} // namespace game::boards
