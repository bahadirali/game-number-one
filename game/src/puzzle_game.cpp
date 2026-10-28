#include "puzzle_game.h"

#include "movement.h"

namespace game {

namespace {

enum class Action {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Quit,
};

constexpr engine::Color kFloorColor{52, 56, 68};
constexpr engine::Color kWallColor{118, 98, 82};
constexpr engine::Color kBlockColor{86, 152, 172};
constexpr engine::Color kPlayerColor{236, 196, 84};

engine::Rect cell_rect(Cell cell, float inset = 0.0f) {
    return {
        static_cast<float>(cell.x * kCellSize) + inset,
        static_cast<float>(cell.y * kCellSize) + inset,
        static_cast<float>(kCellSize) - 2.0f * inset,
        static_cast<float>(kCellSize) - 2.0f * inset,
    };
}

} // namespace

void PuzzleGame::setup(engine::Input& input) {
    using engine::Key;
    input.bind(Action::MoveUp, Key::Up);
    input.bind(Action::MoveUp, Key::W);
    input.bind(Action::MoveDown, Key::Down);
    input.bind(Action::MoveDown, Key::S);
    input.bind(Action::MoveLeft, Key::Left);
    input.bind(Action::MoveLeft, Key::A);
    input.bind(Action::MoveRight, Key::Right);
    input.bind(Action::MoveRight, Key::D);
    input.bind(Action::Quit, Key::Escape);
}

void PuzzleGame::update(const engine::Input& input) {
    if (input.pressed(Action::Quit)) {
        quit_ = true;
        return;
    }

    // At most one step per tick, so holding two directions never moves diagonally.
    struct Binding {
        Action action;
        Direction direction;
    };
    constexpr Binding kMoves[] = {
        {Action::MoveUp, Direction::Up},
        {Action::MoveDown, Direction::Down},
        {Action::MoveLeft, Direction::Left},
        {Action::MoveRight, Direction::Right},
    };
    for (const auto& move : kMoves) {
        if (input.pressed(move.action)) {
            move_player(level_, move.direction);
            break;
        }
    }
}

void PuzzleGame::render(engine::Renderer& renderer) {
    const Grid& grid = level_.grid;
    for (int y = 0; y < grid.height(); ++y) {
        for (int x = 0; x < grid.width(); ++x) {
            const Cell cell{x, y};
            const bool wall = grid.at(cell) == Tile::Wall;
            renderer.fill_rect(cell_rect(cell, wall ? 0.0f : 1.0f), wall ? kWallColor : kFloorColor);
        }
    }
    for (Cell block : level_.blocks) {
        renderer.fill_rect(cell_rect(block, 4.0f), kBlockColor);
    }
    renderer.fill_rect(cell_rect(level_.player, 8.0f), kPlayerColor);
}

} // namespace game
