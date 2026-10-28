#include "match.h"

#include <algorithm>
#include <string>
#include <utility>

#include "level.h"

namespace game {

namespace {

void end_turn(Match& match) {
    --match.moves_left;
    match.current = (match.current + 1) % kPlayerCount;
}

} // namespace

std::optional<Match> parse_match(std::span<const std::string_view> rows, int total_moves) {
    if (total_moves <= 0) {
        return std::nullopt;
    }

    // Let parse_level handle walls, floor and blocks: player 1 stands in for
    // its single '@', and player 2 becomes plain floor.
    std::vector<std::string> translated(rows.begin(), rows.end());
    std::optional<Cell> second;
    int second_count = 0;
    for (std::size_t y = 0; y < translated.size(); ++y) {
        for (std::size_t x = 0; x < translated[y].size(); ++x) {
            char& c = translated[y][x];
            if (c == '@') {
                return std::nullopt;  // '@' belongs to one-player levels
            }
            if (c == '1') {
                c = '@';
            } else if (c == '2') {
                c = '.';
                second = Cell{static_cast<int>(x), static_cast<int>(y)};
                ++second_count;
            }
        }
    }
    if (second_count != 1) {
        return std::nullopt;
    }

    const std::vector<std::string_view> views(translated.begin(), translated.end());
    auto level = parse_level(views);
    if (!level) {
        return std::nullopt;
    }

    return Match{std::move(level->grid), std::move(level->blocks), {level->player, *second}, 0,
                 total_moves};
}

bool is_over(const Match& match) {
    return match.moves_left <= 0;
}

bool play_move(Match& match, Direction direction) {
    if (is_over(match)) {
        return false;
    }
    const int other = (match.current + 1) % kPlayerCount;
    const std::array<Cell, 1> occupied = {match.players[other]};
    if (!move_and_push(match.grid, match.blocks, match.players[match.current], direction,
                       occupied)) {
        return false;
    }
    end_turn(match);
    return true;
}

void pass_turn(Match& match) {
    if (!is_over(match)) {
        end_turn(match);
    }
}

std::vector<Cell> reachable_cells(const Match& match, int player) {
    const auto walkable = [&](Cell cell) {
        return match.grid.at(cell) == Tile::Floor &&
               std::ranges::find(match.blocks, cell) == match.blocks.end() &&
               std::ranges::find(match.players, cell) == match.players.end();
    };

    // Flood fill outwards from the player. `found` doubles as the queue:
    // everything before `next` has had its neighbours checked.
    const Grid& grid = match.grid;
    std::vector<bool> seen(static_cast<std::size_t>(grid.width() * grid.height()), false);
    const auto mark = [&](Cell cell) {
        seen[static_cast<std::size_t>(cell.y * grid.width() + cell.x)] = true;
    };
    const auto is_seen = [&](Cell cell) {
        return seen[static_cast<std::size_t>(cell.y * grid.width() + cell.x)];
    };

    const Cell start = match.players[static_cast<std::size_t>(player)];
    std::vector<Cell> found = {start};
    mark(start);
    for (std::size_t next = 0; next < found.size(); ++next) {
        for (Direction direction :
             {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            const Cell neighbour = step(found[next], direction);
            if (walkable(neighbour) && !is_seen(neighbour)) {
                mark(neighbour);
                found.push_back(neighbour);
            }
        }
    }
    return found;
}

std::optional<int> leader(const Match& match) {
    const auto first = reachable_cells(match, 0).size();
    const auto second = reachable_cells(match, 1).size();
    if (first == second) {
        return std::nullopt;
    }
    return first > second ? 0 : 1;
}

} // namespace game
