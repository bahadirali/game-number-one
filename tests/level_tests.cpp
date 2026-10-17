#include <doctest/doctest.h>

#include <array>
#include <string_view>

#include "level.h"

using game::Cell;
using game::Tile;

TEST_CASE("parse_level reads walls, floor and the player") {
    constexpr std::array<std::string_view, 3> rows = {
        "####",
        "#@.#",
        "####",
    };
    const auto level = game::parse_level(rows);
    REQUIRE(level.has_value());
    CHECK(level->grid.width() == 4);
    CHECK(level->grid.height() == 3);
    CHECK(level->player == Cell{1, 1});
    CHECK(level->grid.at({0, 0}) == Tile::Wall);
    CHECK(level->grid.at({1, 1}) == Tile::Floor);
    CHECK(level->grid.at({2, 1}) == Tile::Floor);
}

TEST_CASE("parse_level pads short rows with walls") {
    constexpr std::array<std::string_view, 2> rows = {
        "@..",
        ".",
    };
    const auto level = game::parse_level(rows);
    REQUIRE(level.has_value());
    CHECK(level->grid.at({0, 1}) == Tile::Floor);
    CHECK(level->grid.at({1, 1}) == Tile::Wall);
}

TEST_CASE("parse_level rejects bad input") {
    constexpr std::array<std::string_view, 1> no_player = {"#..#"};
    constexpr std::array<std::string_view, 1> two_players = {"@.@"};
    constexpr std::array<std::string_view, 1> unknown_char = {"@x"};

    CHECK_FALSE(game::parse_level(no_player).has_value());
    CHECK_FALSE(game::parse_level(two_players).has_value());
    CHECK_FALSE(game::parse_level(unknown_char).has_value());
    CHECK_FALSE(game::parse_level({}).has_value());
}
