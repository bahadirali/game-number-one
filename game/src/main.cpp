#include <array>
#include <cstdio>
#include <string_view>
#include <utility>

#include <engine/app.h>

#include "level.h"
#include "puzzle_game.h"

namespace {

// A hard-coded test room until levels are loaded from files.
constexpr std::array<std::string_view, 9> kLevelRows = {
    "############",
    "#..........#",
    "#..###.....#",
    "#....#..#..#",
    "#.@..#..#..#",
    "#.......##.#",
    "#..##......#",
    "#..........#",
    "############",
};

} // namespace

int main() {
    auto level = game::parse_level(kLevelRows);
    if (!level) {
        std::fprintf(stderr, "The built-in level is invalid.\n");
        return 1;
    }

    engine::AppConfig config;
    config.title = "Game Number One";
    config.width = level->grid.width() * game::kCellSize;
    config.height = level->grid.height() * game::kCellSize;
    config.clear_color = {24, 26, 32};

    game::PuzzleGame puzzle(std::move(*level));
    return engine::run(config, puzzle);
}
