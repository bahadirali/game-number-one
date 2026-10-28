#include "ai.h"

#include <algorithm>
#include <array>
#include <limits>
#include <vector>

namespace game {

namespace {

// Every choice a player has: four steps, then a pass (nothing).
constexpr std::array<std::optional<Direction>, 5> kChoices = {
    Direction::Up, Direction::Down, Direction::Left, Direction::Right, std::nullopt,
};

// Plays `choice` for the current player. Returns false if it's a step that
// goes nowhere, which isn't a legal move.
bool apply(Match& match, std::optional<Direction> choice) {
    if (choice) {
        return play_move(match, *choice);
    }
    pass_turn(match);
    return true;
}

bool is_free(const Match& match, Cell cell) {
    return match.grid.at(cell) == Tile::Floor &&
           std::ranges::find(match.blocks, cell) == match.blocks.end() &&
           std::ranges::find(match.players, cell) == match.players.end();
}

// Whether `player`, standing on `from`, could push a block they don't own
// one step in `direction`.
bool can_claim(const Match& match, int player, Cell from, Direction direction) {
    const Cell block_cell = step(from, direction);
    const auto block = std::ranges::find(match.blocks, block_cell);
    if (block == match.blocks.end()) {
        return false;
    }
    const auto index = static_cast<std::size_t>(block - match.blocks.begin());
    return match.block_owners[index] != player && is_free(match, step(block_cell, direction));
}

// Steps `player` needs to reach a cell from which they can push a block they
// don't own, or `none` if there's no such cell. A breadth-first search, so
// the first cell found is the nearest.
int distance_to_claim(const Match& match, int player, int none) {
    const Grid& grid = match.grid;
    const auto index = [&](Cell cell) {
        return static_cast<std::size_t>(cell.y * grid.width() + cell.x);
    };
    std::vector<int> distance(static_cast<std::size_t>(grid.width() * grid.height()), -1);

    const Cell start = match.players[static_cast<std::size_t>(player)];
    std::vector<Cell> queue = {start};
    distance[index(start)] = 0;
    for (std::size_t next = 0; next < queue.size(); ++next) {
        const Cell cell = queue[next];
        for (const auto& choice : kChoices) {
            if (choice && can_claim(match, player, cell, *choice)) {
                return distance[index(cell)];
            }
        }
        for (const auto& choice : kChoices) {
            if (!choice) {
                continue;
            }
            const Cell neighbour = step(cell, *choice);
            if (is_free(match, neighbour) && distance[index(neighbour)] < 0) {
                distance[index(neighbour)] = distance[index(cell)] + 1;
                queue.push_back(neighbour);
            }
        }
    }
    return none;
}

// How good `match` looks for `player`: mostly the score difference, with the
// walking distance to the next block to claim as a tie-breaker. A cell is
// worth more than any distance on the board, so the score always comes first.
int evaluate(const Match& match, int player) {
    const int cell_count = match.grid.width() * match.grid.height();
    const int opponent = (player + 1) % kPlayerCount;
    const int lead = score(match, player) - score(match, opponent);
    return lead * (cell_count + 1) - distance_to_claim(match, player, cell_count);
}

} // namespace

std::optional<Direction> choose_move(const Match& match) {
    const int me = match.current;
    std::optional<Direction> best_choice;  // passing is always legal
    int best_value = std::numeric_limits<int>::min();

    for (const auto& choice : kChoices) {
        Match after = match;
        if (!apply(after, choice)) {
            continue;
        }

        // Assume the opponent answers with whatever is worst for us.
        int value = std::numeric_limits<int>::max();
        if (is_over(after)) {
            value = evaluate(after, me);
        } else {
            for (const auto& reply : kChoices) {
                Match replied = after;
                if (apply(replied, reply)) {
                    value = std::min(value, evaluate(replied, me));
                }
            }
        }

        // Strictly better only, so ties keep the earlier choice and a step
        // wins over passing.
        if (value > best_value) {
            best_value = value;
            best_choice = choice;
        }
    }
    return best_choice;
}

} // namespace game
