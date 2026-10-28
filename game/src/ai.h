#pragma once

#include <optional>

#include "match.h"

namespace game {

// Picks a move for the player whose turn it is: a direction to step in, or
// nothing to pass.
//
// It looks two moves ahead (its own move, then the opponent's best reply)
// and prefers positions where it controls more cells than the opponent.
// When no line of play changes the score, it walks towards the nearest block
// it could push but doesn't own yet, so it never just wanders.
std::optional<Direction> choose_move(const Match& match);

} // namespace game
