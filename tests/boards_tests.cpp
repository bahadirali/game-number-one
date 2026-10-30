#include <doctest/doctest.h>

#include "ai.h"
#include "boards.h"
#include "level.h"
#include "match.h"

using game::Cell;
using game::Direction;
namespace boards = game::boards;

TEST_CASE("the built-in boards are valid") {
    CHECK(game::parse_level(boards::kPuzzleRows).has_value());
    CHECK(game::parse_match(boards::kDuelRows, boards::kDuelMoves).has_value());
    CHECK(game::parse_match(boards::kStuckTestRows, boards::kStuckTestMoves).has_value());
}

TEST_CASE("the stuck-test board plays out as described") {
    auto match = *game::parse_match(boards::kStuckTestRows, boards::kStuckTestMoves);

    // Player 1 has exactly one move: left.
    CHECK_FALSE(game::play_move(match, Direction::Up));
    CHECK_FALSE(game::play_move(match, Direction::Down));
    CHECK_FALSE(game::play_move(match, Direction::Right));
    CHECK(game::play_move(match, Direction::Left));

    // Player 2 has exactly one move: left, which shuts player 1 in. The AI
    // finds it too, rather than passing.
    CHECK(game::choose_move(match) == Direction::Left);
    CHECK_FALSE(game::play_move(match, Direction::Up));
    CHECK_FALSE(game::play_move(match, Direction::Down));
    CHECK_FALSE(game::play_move(match, Direction::Right));
    CHECK(game::play_move(match, Direction::Left));
    CHECK(match.players[1] == Cell{8, 1});

    CHECK_FALSE(game::can_move(match));
    CHECK(game::pass_if_stuck(match));
    CHECK(match.current == 1);
}

TEST_CASE("on the stuck-test board, player 2 is stuck at once when starting") {
    auto match = game::with_first_player(
        *game::parse_match(boards::kStuckTestRows, boards::kStuckTestMoves), 1);
    CHECK(game::pass_if_stuck(match));
    CHECK(match.current == 0);
}
