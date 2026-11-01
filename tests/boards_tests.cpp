#include <doctest/doctest.h>

#include "boards.h"
#include "level.h"

TEST_CASE("the built-in puzzle board is valid") {
    CHECK(game::parse_level(game::boards::kPuzzleRows).has_value());
}
