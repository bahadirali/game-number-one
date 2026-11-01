#include <doctest/doctest.h>

#include <filesystem>
#include <fstream>
#include <string>

#include "ai.h"
#include "level_file.h"

using game::Cell;
using game::Direction;

TEST_CASE("parse_match_file reads the move count and the board") {
    const auto match = game::parse_match_file("moves 12\n#####\n#1$2#\n#####\n");
    REQUIRE(match.has_value());
    CHECK(match->moves_left == 12);
    CHECK(match->players[0] == Cell{1, 1});
    CHECK(match->players[1] == Cell{3, 1});
    CHECK(match->blocks.size() == 1);
}

TEST_CASE("parse_match_file accepts Windows line endings and trailing blank lines") {
    const auto match = game::parse_match_file("moves 4\r\n#1.2#\r\n\r\n\r\n");
    REQUIRE(match.has_value());
    CHECK(match->grid.width() == 5);
    CHECK(match->grid.height() == 1);
}

TEST_CASE("parse_match_file rejects a missing or malformed header") {
    CHECK_FALSE(game::parse_match_file("").has_value());
    CHECK_FALSE(game::parse_match_file("#1.2#\n").has_value());           // no header
    CHECK_FALSE(game::parse_match_file("moves 4\n").has_value());         // no board
    CHECK_FALSE(game::parse_match_file("moves\n#1.2#\n").has_value());    // no number
    CHECK_FALSE(game::parse_match_file("moves 4x\n#1.2#\n").has_value()); // junk after it
    CHECK_FALSE(game::parse_match_file("moves 0\n#1.2#\n").has_value());  // not positive
}

TEST_CASE("load_match_file reports a missing file") {
    CHECK_FALSE(game::load_match_file(GAME_LEVELS_DIR "no_such_level.txt").has_value());
}

TEST_CASE("load_match_file finds files in folders with non-ASCII names") {
    namespace fs = std::filesystem;
    // A folder named "Çağrı", written as UTF-8 bytes so this file's own
    // encoding doesn't matter.
    const std::u8string folder_name = u8"Çağrı";
    const fs::path folder = fs::temp_directory_path() / "game_level_test" / folder_name;
    fs::create_directories(folder);
    std::ofstream(folder / "level.txt") << "moves 4\n#1.2#\n";

    // Hand the path over as a UTF-8 std::string, as engine::base_path() gives it.
    const std::u8string path = (folder / "level.txt").u8string();
    const std::string path_utf8(reinterpret_cast<const char*>(path.data()), path.size());
    CHECK(game::load_match_file(path_utf8).has_value());

    fs::remove_all(folder.parent_path());
}

TEST_CASE("the shipped duel level loads") {
    const auto match = game::load_match_file(GAME_LEVELS_DIR "duel.txt");
    REQUIRE(match.has_value());
    CHECK(match->moves_left == 50);
    CHECK(match->blocks.size() == 24);
}

// The stuck-test level exists to try out the automatic pass by hand, so
// check it really plays out that way.
TEST_CASE("the stuck-test level shuts player 1 in on player 2's only move") {
    auto loaded = game::load_match_file(GAME_LEVELS_DIR "stuck_test.txt");
    REQUIRE(loaded.has_value());
    auto match = *loaded;

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

    CHECK_FALSE(game::can_move(match));
    CHECK(game::pass_if_stuck(match));
    CHECK(match.current == 1);

    // In a round where player 2 starts, they're stuck from the first turn.
    auto swapped = game::with_first_player(*loaded, 1);
    CHECK(game::pass_if_stuck(swapped));
}
