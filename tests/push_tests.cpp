#include <doctest/doctest.h>

#include <array>
#include <string_view>

#include "level.h"
#include "movement.h"

using game::Cell;
using game::Direction;

namespace {

template <std::size_t N>
game::Level make_level(const std::array<std::string_view, N>& rows) {
    auto level = game::parse_level(rows);
    REQUIRE(level.has_value());
    return *level;
}

} // namespace

TEST_CASE("walking into a block pushes it onto free floor") {
    auto level = make_level(std::array<std::string_view, 3>{
        "#####",
        "#@$.#",
        "#####",
    });
    CHECK(game::move_player(level, Direction::Right));
    CHECK(level.player == Cell{2, 1});
    REQUIRE(level.blocks.size() == 1);
    CHECK(level.blocks[0] == Cell{3, 1});
}

TEST_CASE("a block against a wall can't be pushed") {
    auto level = make_level(std::array<std::string_view, 3>{
        "####",
        "#@$#",
        "####",
    });
    CHECK_FALSE(game::move_player(level, Direction::Right));
    CHECK(level.player == Cell{1, 1});
    CHECK(level.blocks[0] == Cell{2, 1});
}

TEST_CASE("a block can't push another block") {
    auto level = make_level(std::array<std::string_view, 3>{
        "######",
        "#@$$.#",
        "######",
    });
    CHECK_FALSE(game::move_player(level, Direction::Right));
    CHECK(level.player == Cell{1, 1});
    CHECK(level.blocks[0] == Cell{2, 1});
    CHECK(level.blocks[1] == Cell{3, 1});
}

TEST_CASE("a block at the grid edge can't leave the grid") {
    auto level = make_level(std::array<std::string_view, 1>{"@$"});
    CHECK_FALSE(game::move_player(level, Direction::Right));
    CHECK(level.blocks[0] == Cell{1, 0});
}

TEST_CASE("pushing works in every direction") {
    //   .....
    //   ..$..
    //   .$@$.
    //   ..$..
    //   .....
    const std::array<std::string_view, 5> rows = {
        ".....",
        "..$..",
        ".$@$.",
        "..$..",
        ".....",
    };
    struct Case {
        Direction direction;
        Cell block_from;
        Cell block_to;
    };
    const Case cases[] = {
        {Direction::Up, {2, 1}, {2, 0}},
        {Direction::Down, {2, 3}, {2, 4}},
        {Direction::Left, {1, 2}, {0, 2}},
        {Direction::Right, {3, 2}, {4, 2}},
    };
    for (const Case& c : cases) {
        auto level = make_level(rows);
        CHECK(game::move_player(level, c.direction));
        CHECK(level.player == c.block_from);
        CHECK_FALSE(game::has_block(level, c.block_from));
        CHECK(game::has_block(level, c.block_to));
    }
}

TEST_CASE("walls still block the player when no block is involved") {
    auto level = make_level(std::array<std::string_view, 3>{
        "###",
        "#@#",
        "###",
    });
    CHECK_FALSE(game::move_player(level, Direction::Left));
    CHECK(level.player == Cell{1, 1});
}

TEST_CASE("a pushed block keeps moving with repeated pushes until it hits a wall") {
    auto level = make_level(std::array<std::string_view, 3>{
        "######",
        "#@$..#",
        "######",
    });
    CHECK(game::move_player(level, Direction::Right));
    CHECK(game::move_player(level, Direction::Right));
    CHECK_FALSE(game::move_player(level, Direction::Right));
    CHECK(level.player == Cell{3, 1});
    CHECK(level.blocks[0] == Cell{4, 1});
}
