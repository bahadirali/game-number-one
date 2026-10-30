#include <cstdio>
#include <cstring>
#include <span>
#include <string_view>
#include <utility>

#include <engine/app.h>

#include "boards.h"
#include "duel_game.h"
#include "layout.h"
#include "level.h"
#include "match.h"
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

int run_duel(std::span<const std::string_view> rows, int total_moves) {
    auto match = game::parse_match(rows, total_moves);
    if (!match) {
        std::fprintf(stderr, "The built-in duel board is invalid.\n");
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
        return run_duel(game::boards::kStuckTestRows, game::boards::kStuckTestMoves);
    }
    return run_duel(game::boards::kDuelRows, game::boards::kDuelMoves);
}
