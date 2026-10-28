#pragma once

#include <array>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include "grid.h"
#include "movement.h"

namespace game {

constexpr int kPlayerCount = 2;

// A two-player, turn-based round on one shared board. Players take turns
// making one move each: a step (which may push a block) or a pass. When the
// moves run out the round ends, and each player scores the empty cells they
// can still walk to. Cells both players can reach count for both, so only
// walled-off space decides the winner.
struct Match {
    Grid grid;
    std::vector<Cell> blocks;
    std::array<Cell, kPlayerCount> players;
    int current = 0;     // index of the player whose turn it is
    int moves_left = 0;  // moves remaining for both players together
};

// Builds a match from rows of text, like parse_level but with players
// '1' and '2' instead of '@'. Returns nothing if parse_level would reject
// the board, if there isn't exactly one of each player, or if
// `total_moves` isn't positive.
std::optional<Match> parse_match(std::span<const std::string_view> rows, int total_moves);

bool is_over(const Match& match);

// Moves the current player one step, pushing blocks like move_player. The
// other player blocks the way like a wall. A move that goes nowhere doesn't
// use up the turn. Returns whether the player moved.
bool play_move(Match& match, Direction direction);

// Ends the current player's turn without moving.
void pass_turn(Match& match);

// Cells `player` can walk to: floor with no block and no other player on it,
// connected through such cells. Includes the player's own cell.
std::vector<Cell> reachable_cells(const Match& match, int player);

// The player who reaches more cells right now, or nothing on a tie.
// Once is_over() is true this is the winner.
std::optional<int> leader(const Match& match);

} // namespace game
