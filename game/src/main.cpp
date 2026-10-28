#include <array>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <utility>

#include <engine/app.h>

#include "duel_game.h"
#include "layout.h"
#include "level.h"
#include "match.h"
#include "puzzle_game.h"

namespace {

// Hard-coded boards until levels are loaded from files.
constexpr std::array<std::string_view, 9> kPuzzleRows = {
    "############",
    "#..........#",
    "#..###..$..#",
    "#....#..#..#",
    "#.@$.#..#..#",
    "#....$..##.#",
    "#..##...$..#",
    "#..........#",
    "############",
};

// Point-symmetric, so neither player starts with an edge.
constexpr std::array<std::string_view, 9> kDuelRows = {
    "############",
    "#1.........#",
    "#..$....$..#",
    "#...#..#...#",
    "#.$......$.#",
    "#...#..#...#",
    "#..$....$..#",
    "#.........2#",
    "############",
};
constexpr int kDuelMoves = 30;  // 15 each

engine::AppConfig make_config(const game::Grid& grid, int extra_rows) {
    engine::AppConfig config;
    config.title = "Game Number One";
    config.width = grid.width() * game::kCellSize;
    config.height = (grid.height() + extra_rows) * game::kCellSize;
    config.clear_color = {24, 26, 32};
    return config;
}

int run_puzzle() {
    auto level = game::parse_level(kPuzzleRows);
    if (!level) {
        std::fprintf(stderr, "The built-in puzzle level is invalid.\n");
        return 1;
    }
    const auto config = make_config(level->grid, 0);
    game::PuzzleGame puzzle(std::move(*level));
    return engine::run(config, puzzle);
}

int run_duel() {
    auto match = game::parse_match(kDuelRows, kDuelMoves);
    if (!match) {
        std::fprintf(stderr, "The built-in duel board is invalid.\n");
        return 1;
    }
    const auto config = make_config(match->grid, 1);  // one extra row for the move strip
    game::DuelGame duel(std::move(*match));
    return engine::run(config, duel);
}

} // namespace

// Starts the two-player duel; pass --puzzle for the one-player puzzle room.
int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--puzzle") == 0) {
        return run_puzzle();
    }
    return run_duel();
}
