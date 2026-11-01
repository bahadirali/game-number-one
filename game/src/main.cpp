#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

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

// Plays the duel levels in levels/, next to the executable, in the given order.
int run_duel(const std::vector<std::string>& file_names) {
    std::vector<game::Match> levels;
    for (const std::string& file_name : file_names) {
        const std::string path = engine::base_path() + "levels/" + file_name;
        auto match = game::load_match_file(path);
        if (!match) {
            std::fprintf(stderr, "Couldn't load the level %s\n", path.c_str());
            return 1;
        }
        // The window is sized once, for the first level.
        if (!levels.empty() && (match->grid.width() != levels.front().grid.width() ||
                                match->grid.height() != levels.front().grid.height())) {
            std::fprintf(stderr, "The level %s isn't the same size as the first level.\n",
                         path.c_str());
            return 1;
        }
        levels.push_back(std::move(*match));
    }
    if (levels.empty()) {
        std::fprintf(stderr, "There are no levels to play.\n");
        return 1;
    }
    // One extra row for the move and score strips.
    const auto config = make_config(levels.front().grid, 1);
    game::DuelGame duel(std::move(levels));
    return engine::run(config, duel);
}

// Plays the levels listed in levels/levels.txt.
int run_duel_list() {
    const std::string path = engine::base_path() + "levels/levels.txt";
    const auto file_names = game::load_level_list(path);
    if (!file_names) {
        std::fprintf(stderr, "Couldn't read the level list %s\n", path.c_str());
        return 1;
    }
    return run_duel(*file_names);
}

} // namespace

// Starts the two-player duel with the levels in levels/levels.txt. Options:
//   --puzzle      the one-player puzzle room instead
//   --stuck-test  a tiny duel board for trying out the automatic pass
int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--puzzle") == 0) {
        return run_puzzle();
    }
    if (argc > 1 && std::strcmp(argv[1], "--stuck-test") == 0) {
        return run_duel({"stuck_test.txt"});
    }
    return run_duel_list();
}
