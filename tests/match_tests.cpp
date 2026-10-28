#include <doctest/doctest.h>

#include <array>
#include <string_view>

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

TEST_CASE("reachable_cells floods open floor around blocks, walls and the other player") {
    const auto match = make_match(std::array<std::string_view, 4>{
        "#######",
        "#1.$..#",
        "#..#2.#",
        "#######",
    });
    // Player 1's side: (1,1) (2,1) (1,2) (2,2). The block and wall seal it off.
    CHECK(game::reachable_cells(match, 0).size() == 4);
    // Player 2's side: its own cell plus (4,1) (5,1) (5,2).
    CHECK(game::reachable_cells(match, 1).size() == 4);
    CHECK_FALSE(game::leader(match).has_value());
}

TEST_CASE("leader is whoever reaches more cells") {
    const auto match = make_match(std::array<std::string_view, 3>{
        "#######",
        "#1..$2#",
        "#######",
    });
    CHECK(game::reachable_cells(match, 0).size() == 3);
    CHECK(game::reachable_cells(match, 1).size() == 1);
    CHECK(game::leader(match) == 0);
}

TEST_CASE("cells both players reach count for both") {
    const auto match = make_match(std::array<std::string_view, 3>{
        "######",
        "#1..2#",
        "######",
    });
    CHECK(game::reachable_cells(match, 0).size() == 3);
    CHECK(game::reachable_cells(match, 1).size() == 3);
    CHECK_FALSE(game::leader(match).has_value());
}
