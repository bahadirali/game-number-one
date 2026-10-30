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

// The pusher owns the pushed block, and flips any opponent blocks it now
// touches side by side. Neutral blocks it touches stay neutral, and flipped
// blocks don't flip further blocks.
void claim_pushed_block(Match& match, std::size_t index) {
    match.block_owners[index] = match.current;
    const Cell pushed = match.blocks[index];
    for (Direction direction : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
        const auto touched = std::ranges::find(match.blocks, step(pushed, direction));
        if (touched == match.blocks.end()) {
            continue;
        }
        int& owner = match.block_owners[static_cast<std::size_t>(touched - match.blocks.begin())];
        if (owner != kNoOwner) {
            owner = match.current;
        }
    }
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

    // Blocks start out neutral; pushing one claims it.
    std::vector<int> owners(level->blocks.size(), kNoOwner);
    return Match{std::move(level->grid), std::move(level->blocks), std::move(owners),
                 {level->player, *second}, 0, total_moves};
}

Match with_first_player(Match start, int first_player) {
    start.current = first_player;
    return start;
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

    // Note which block (if any) is in the way: if the move succeeds, it was pushed.
    Cell& mover = match.players[match.current];
    const auto pushed = std::ranges::find(match.blocks, step(mover, direction));
    const auto pushed_index = pushed - match.blocks.begin();

    if (!move_and_push(match.grid, match.blocks, mover, direction, occupied)) {
        return false;
    }
    if (pushed != match.blocks.end()) {
        claim_pushed_block(match, static_cast<std::size_t>(pushed_index));
    }
    end_turn(match);
    return true;
}

void pass_turn(Match& match) {
    if (!is_over(match)) {
        end_turn(match);
    }
}

bool can_move(const Match& match) {
    for (Direction direction : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
        Match trial = match;
        if (play_move(trial, direction)) {
            return true;
        }
    }
    return false;
}

bool pass_if_stuck(Match& match) {
    if (is_over(match) || can_move(match)) {
        return false;
    }
    pass_turn(match);
    return true;
}

std::optional<int> controller(const Match& match, Cell cell) {
    if (match.grid.at(cell) != Tile::Floor) {
        return std::nullopt;
    }

    std::array<int, kPlayerCount> nearby{};
    for (std::size_t i = 0; i < match.blocks.size(); ++i) {
        const Cell block = match.blocks[i];
        const int dx = block.x - cell.x;
        const int dy = block.y - cell.y;
        const bool neighbour = (dx != 0 || dy != 0) && dx >= -1 && dx <= 1 && dy >= -1 && dy <= 1;
        if (neighbour && match.block_owners[i] != kNoOwner) {
            ++nearby[static_cast<std::size_t>(match.block_owners[i])];
        }
    }

    if (nearby[0] == nearby[1]) {
        return std::nullopt;
    }
    return nearby[0] > nearby[1] ? 0 : 1;
}

int score(const Match& match, int player) {
    int total = 0;
    for (int y = 0; y < match.grid.height(); ++y) {
        for (int x = 0; x < match.grid.width(); ++x) {
            if (controller(match, {x, y}) == player) {
                ++total;
            }
        }
    }
    return total;
}

std::optional<int> leader(const Match& match) {
    const int first = score(match, 0);
    const int second = score(match, 1);
    if (first == second) {
        return std::nullopt;
    }
    return first > second ? 0 : 1;
}

} // namespace game
