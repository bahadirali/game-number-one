#include <doctest/doctest.h>

#include <array>
#include <string_view>

#include "ai.h"

using game::Direction;

namespace {

template <std::size_t N>
game::Match make_match(const std::array<std::string_view, N>& rows) {
    auto match = game::parse_match(rows, 20);
    REQUIRE(match.has_value());
    return *match;
}

} // namespace

TEST_CASE("the AI pushes a block when that wins it cells") {
    const auto match = make_match(std::array<std::string_view, 5>{
        "########",
        "#1$....#",
        "#......#",
        "#.....2#",
        "########",
    });
    CHECK(game::choose_move(match) == Direction::Right);
}

TEST_CASE("the AI walks towards a block it can claim when nothing scores yet") {
    // No push is possible this turn; Right and Down both get closer to the block.
    const auto match = make_match(std::array<std::string_view, 6>{
        "########",
        "#1.....#",
        "#......#",
        "#....$.#",
        "#......#",
        "#######2",
    });
    const auto move = game::choose_move(match);
    CHECK((move == Direction::Right || move == Direction::Down));
}

TEST_CASE("the AI takes an opponent block back rather than a neutral one") {
    // Pushing (2,1) right steals it from P2; pushing (1,2) down claims a
    // neutral block. Stealing gains P1 the same cells and also costs P2 theirs.
    auto match = make_match(std::array<std::string_view, 6>{
        "#######",
        "#1$...#",
        "#$....#",
        "#.....#",
        "#.....#",
        "#....2#",
    });
    match.block_owners = {1, game::kNoOwner};
    CHECK(game::choose_move(match) == Direction::Right);
}

TEST_CASE("the AI passes when it can't move") {
    const auto match = make_match(std::array<std::string_view, 3>{
        "#####",
        "#1#2#",
        "#####",
    });
    CHECK_FALSE(game::choose_move(match).has_value());
}
