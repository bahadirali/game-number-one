#include <doctest/doctest.h>

#include <array>
#include <string_view>
#include <vector>

#include "match.h"

using game::Cell;
using game::Direction;

namespace {

template <std::size_t N>
game::Match make_match(const std::array<std::string_view, N>& rows, int total_moves = 10) {
    auto match = game::parse_match(rows, total_moves);
    REQUIRE(match.has_value());
    return *match;
}

} // namespace

TEST_CASE("parse_match finds both players and starts with player 1") {
    const auto match = make_match(std::array<std::string_view, 3>{
        "#####",
        "#1$2#",
        "#####",
    }, 6);
    CHECK(match.players[0] == Cell{1, 1});
    CHECK(match.players[1] == Cell{3, 1});
    REQUIRE(match.blocks.size() == 1);
    CHECK(match.blocks[0] == Cell{2, 1});
    CHECK(match.current == 0);
    CHECK(match.moves_left == 6);
    CHECK_FALSE(game::is_over(match));
}

TEST_CASE("parse_match rejects boards without exactly one of each player") {
    constexpr std::array<std::string_view, 1> no_second = {"1.."};
    constexpr std::array<std::string_view, 1> two_seconds = {"12.2"};
    constexpr std::array<std::string_view, 1> one_player_marker = {"1.2@"};
    CHECK_FALSE(game::parse_match(no_second, 4).has_value());
    CHECK_FALSE(game::parse_match(two_seconds, 4).has_value());
    CHECK_FALSE(game::parse_match(one_player_marker, 4).has_value());
}

TEST_CASE("parse_match rejects a nonpositive move count") {
    constexpr std::array<std::string_view, 1> rows = {"1.2"};
    CHECK_FALSE(game::parse_match(rows, 0).has_value());
}

TEST_CASE("players take turns, and only the current player moves") {
    auto match = make_match(std::array<std::string_view, 3>{
        "######",
        "#1..2#",
        "######",
    });
    CHECK(game::play_move(match, Direction::Right));
    CHECK(match.players[0] == Cell{2, 1});
    CHECK(match.players[1] == Cell{4, 1});
    CHECK(match.current == 1);
    CHECK(match.moves_left == 9);

    CHECK(game::play_move(match, Direction::Left));
    CHECK(match.players[1] == Cell{3, 1});
    CHECK(match.current == 0);
}

TEST_CASE("a blocked move keeps the turn") {
    auto match = make_match(std::array<std::string_view, 3>{
        "#####",
        "#1.2#",
        "#####",
    });
    CHECK_FALSE(game::play_move(match, Direction::Up));
    CHECK(match.current == 0);
    CHECK(match.moves_left == 10);
}

TEST_CASE("players can't walk into each other or push a block onto each other") {
    auto match = make_match(std::array<std::string_view, 3>{
        "######",
        "#12$.#",
        "######",
    });
    CHECK_FALSE(game::play_move(match, Direction::Right));
    CHECK(match.players[0] == Cell{1, 1});

    auto pushing = make_match(std::array<std::string_view, 3>{
        "#####",
        "#1$2#",
        "#####",
    });
    CHECK_FALSE(game::play_move(pushing, Direction::Right));
    CHECK(pushing.blocks[0] == Cell{2, 1});
}

TEST_CASE("passing ends the turn without moving") {
    auto match = make_match(std::array<std::string_view, 1>{"1.2"});
    game::pass_turn(match);
    CHECK(match.players[0] == Cell{0, 0});
    CHECK(match.current == 1);
    CHECK(match.moves_left == 9);
}

TEST_CASE("the match ends when the moves run out, and then nobody moves") {
    auto match = make_match(std::array<std::string_view, 1>{"1...2"}, 2);
    CHECK(game::play_move(match, Direction::Right));
    CHECK(game::play_move(match, Direction::Left));
    CHECK(game::is_over(match));
    CHECK_FALSE(game::play_move(match, Direction::Right));
    game::pass_turn(match);
    CHECK(match.moves_left == 0);
    CHECK(match.players[0] == Cell{1, 0});
}

TEST_CASE("blocks start neutral, and pushing one makes it yours") {
    auto match = make_match(std::array<std::string_view, 3>{
        "#######",
        "#1$..2#",
        "#######",
    });
    REQUIRE(match.block_owners.size() == 1);
    CHECK(match.block_owners[0] == game::kNoOwner);

    CHECK(game::play_move(match, Direction::Right));
    CHECK(match.block_owners[0] == 0);
}

TEST_CASE("pushing your opponent's block steals it") {
    auto match = make_match(std::array<std::string_view, 5>{
        "######",
        "#....#",
        "#1$..#",
        "#..2.#",
        "######",
    });
    CHECK(game::play_move(match, Direction::Right));  // P1 pushes the block to (3,2)
    CHECK(match.block_owners[0] == 0);
    CHECK(game::play_move(match, Direction::Up));     // P2 pushes it on to (3,1)
    CHECK(match.blocks[0] == Cell{3, 1});
    CHECK(match.block_owners[0] == 1);
}

TEST_CASE("a pushed block flips the opponent blocks it touches side by side") {
    // P1 pushes the block at (2,2) to (3,2). There it touches (4,2) on the
    // right and (3,1) above, but (4,3) only diagonally.
    auto match = make_match(std::array<std::string_view, 5>{
        "#######",
        "#..$..#",
        "#1$.$.#",
        "#...$2#",
        "#######",
    });
    // Blocks in reading order: (3,1), (2,2), (4,2), (4,3).
    match.block_owners = {1, game::kNoOwner, 1, 1};

    CHECK(game::play_move(match, Direction::Right));
    CHECK(match.blocks[1] == Cell{3, 2});
    CHECK(match.block_owners[0] == 0);  // above: flipped
    CHECK(match.block_owners[1] == 0);  // the pushed block
    CHECK(match.block_owners[2] == 0);  // right: flipped
    CHECK(match.block_owners[3] == 1);  // diagonal: untouched, and no chain from (4,2)
}

TEST_CASE("touching neutral or your own blocks doesn't change them") {
    // P1 pushes (2,2) to (3,2), touching (3,1) above and (4,2) on the right.
    auto match = make_match(std::array<std::string_view, 4>{
        "#######",
        "#..$..#",
        "#1$.$2#",
        "#######",
    });
    // Blocks in reading order: (3,1), (2,2), (4,2).
    match.block_owners = {game::kNoOwner, game::kNoOwner, 0};
    CHECK(game::play_move(match, Direction::Right));
    CHECK(match.block_owners == std::vector<int>{game::kNoOwner, 0, 0});
}

TEST_CASE("a failed push doesn't change the owner") {
    auto match = make_match(std::array<std::string_view, 3>{
        "#####",
        "#1$#2",
        "#####",
    });
    CHECK_FALSE(game::play_move(match, Direction::Right));
    CHECK(match.block_owners[0] == game::kNoOwner);
}

TEST_CASE("an owned block controls the floor around it, but not walls or itself") {
    auto match = make_match(std::array<std::string_view, 5>{
        "#####",
        "#...#",
        "#.$.#",
        "#1.2#",
        "#####",
    });
    CHECK_FALSE(game::controller(match, {1, 1}).has_value());  // neutral block
    match.block_owners[0] = 0;

    CHECK(game::controller(match, {1, 1}) == 0);  // diagonal
    CHECK(game::controller(match, {2, 1}) == 0);  // straight
    CHECK(game::controller(match, {1, 3}) == 0);  // under a player still counts
    CHECK_FALSE(game::controller(match, {2, 2}).has_value());  // its own cell
    CHECK_FALSE(game::controller(match, {0, 0}).has_value());  // wall
    CHECK(game::score(match, 0) == 8);
    CHECK(game::score(match, 1) == 0);
    CHECK(game::leader(match) == 0);
}

TEST_CASE("a cell near both players' blocks goes to whoever has more of them") {
    // Blocks at (1,1), (3,1) and (1,3) all neighbour the middle cell (2,2).
    auto match = make_match(std::array<std::string_view, 5>{
        "######",
        "#$.$1#",
        "#....#",
        "#$..2#",
        "######",
    });
    const Cell middle{2, 2};

    match.block_owners = {0, 1, 1};
    CHECK(game::controller(match, middle) == 1);

    match.block_owners = {0, 0, 1};
    CHECK(game::controller(match, middle) == 0);

    match.block_owners = {0, 1, game::kNoOwner};  // neutral blocks don't count
    CHECK_FALSE(game::controller(match, middle).has_value());
}

TEST_CASE("with_first_player starts a fresh round with the given player to move") {
    const auto start = make_match(std::array<std::string_view, 1>{"1...2"}, 4);
    auto round = game::with_first_player(start, 1);
    CHECK(round.current == 1);
    CHECK(round.moves_left == 4);
    CHECK(game::play_move(round, Direction::Left));
    CHECK(round.players[1] == Cell{3, 0});
    CHECK(round.current == 0);
}

TEST_CASE("a player boxed in by walls, unpushable blocks and the other player can't move") {
    // Up and down are walls, the block on the left is against a wall, and
    // player 2 is on the right.
    auto match = make_match(std::array<std::string_view, 3>{
        "######",
        "#$12.#",
        "######",
    });
    CHECK_FALSE(game::can_move(match));
    CHECK(game::pass_if_stuck(match));
    CHECK(match.current == 1);
    CHECK(match.moves_left == 9);

    // Player 2 can step right, so nothing is passed for them.
    CHECK(game::can_move(match));
    CHECK_FALSE(game::pass_if_stuck(match));
    CHECK(match.current == 1);
    CHECK(match.moves_left == 9);
}

TEST_CASE("pass_if_stuck does nothing once the round is over") {
    auto match = make_match(std::array<std::string_view, 1>{"#12#"}, 1);
    game::pass_turn(match);
    REQUIRE(game::is_over(match));
    CHECK_FALSE(game::pass_if_stuck(match));
    CHECK(match.moves_left == 0);
}
