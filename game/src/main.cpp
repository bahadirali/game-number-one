#include <cstdio>
#include <cstring>
#include <string>
#include <utility>

#include <engine/app.h>

#include "boards.h"
#include "duel_game.h"
#include "layout.h"
#include "level.h"
#include "level_file.h"
#include "puzzle_game.h"

namespace {

engine::AppConfig make_config(const game::Grid& grid, int extra_rows) {
    engine::AppConfig config;
    config.title = "Game Number One";
    config.width = grid.width() * game::kCellSize;
    config.height = (grid.height() + extra_rows) * game::kCellSize;
    config.clear_color = {24, 26, 32};
    return config;
}

int run_puzzle() {
    auto level = game::parse_level(game::boards::kPuzzleRows);
    if (!level) {
        std::fprintf(stderr, "The built-in puzzle level is invalid.\n");
        return 1;
    }
    const auto config = make_config(level->grid, 0);
    game::PuzzleGame puzzle(std::move(*level));
    return engine::run(config, puzzle);
}

// Plays the duel level in levels/<file_name>, next to the executable.
int run_duel(const std::string& file_name) {
    const std::string path = engine::base_path() + "levels/" + file_name;
    auto match = game::load_match_file(path);
    if (!match) {
        std::fprintf(stderr, "Couldn't load the level %s\n", path.c_str());
        return 1;
    }
    const auto config = make_config(match->grid, 1);  // one extra row for the move and score strips
    game::DuelGame duel(std::move(*match));
    return engine::run(config, duel);
}

} // namespace

// Starts the two-player duel. Options:
//   --puzzle      the one-player puzzle room instead
//   --stuck-test  a tiny duel board for trying out the automatic pass
int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--puzzle") == 0) {
        return run_puzzle();
    }
    if (argc > 1 && std::strcmp(argv[1], "--stuck-test") == 0) {
        return run_duel("stuck_test.txt");
    }
    return run_duel("duel.txt");
}
