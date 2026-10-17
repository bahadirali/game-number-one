#include <doctest/doctest.h>

#include <array>
#include <string_view>

#include "level.h"
#include "movement.h"

using game::Cell;
using game::Direction;

namespace {

// Player in the middle of a 3x3 floor, with walls all around.
//   #####
//   #...#
//   #.@.#
//   #..##   <- wall at (3, 3)
//   #####
game::Level small_room() {
    constexpr std::array<std::string_view, 5> rows = {
        "#####",
        "#...#",
        "#.@.#",
        "#..##",
        "#####",
    };
    auto level = game::parse_level(rows);
    REQUIRE(level.has_value());
    return *level;
}

} // namespace

TEST_CASE("step moves one cell in each direction") {
    const Cell from{5, 5};
    CHECK(game::step(from, Direction::Up) == Cell{5, 4});
    CHECK(game::step(from, Direction::Down) == Cell{5, 6});
    CHECK(game::step(from, Direction::Left) == Cell{4, 5});
    CHECK(game::step(from, Direction::Right) == Cell{6, 5});
}

TEST_CASE("player moves onto floor") {
    const auto level = small_room();
    CHECK(game::try_move(level.grid, level.player, Direction::Up) == Cell{2, 1});
    CHECK(game::try_move(level.grid, level.player, Direction::Down) == Cell{2, 3});
    CHECK(game::try_move(level.grid, level.player, Direction::Left) == Cell{1, 2});
    CHECK(game::try_move(level.grid, level.player, Direction::Right) == Cell{3, 2});
}

TEST_CASE("walls block the player") {
    const auto level = small_room();

    SUBCASE("outer wall") {
        const Cell top_left{1, 1};
        CHECK(game::try_move(level.grid, top_left, Direction::Up) == top_left);
        CHECK(game::try_move(level.grid, top_left, Direction::Left) == top_left);
    }

    SUBCASE("inner wall") {
        const Cell above_wall{3, 2};
        CHECK(game::try_move(level.grid, above_wall, Direction::Down) == above_wall);
    }
}

TEST_CASE("cells outside the grid count as walls") {
    game::Grid grid(2, 1);  // all floor, no walls at the edge
    CHECK(grid.at({-1, 0}) == game::Tile::Wall);
    CHECK(grid.at({2, 0}) == game::Tile::Wall);
    CHECK(game::try_move(grid, {0, 0}, Direction::Left) == Cell{0, 0});
    CHECK(game::try_move(grid, {0, 0}, Direction::Up) == Cell{0, 0});
    CHECK(game::try_move(grid, {0, 0}, Direction::Right) == Cell{1, 0});
}

TEST_CASE("a sequence of moves walks around the room") {
    const auto level = small_room();
    Cell player = level.player;
    for (Direction d : {Direction::Right, Direction::Right, Direction::Down, Direction::Left}) {
        player = game::try_move(level.grid, player, d);
    }
    // Right, then blocked by the outer wall, then blocked by the inner wall, then left.
    CHECK(player == Cell{2, 2});
}
