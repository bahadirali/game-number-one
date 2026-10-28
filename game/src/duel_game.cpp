#include "duel_game.h"

#include <cstdio>

#include "ai.h"
#include "layout.h"

namespace game {

namespace {

enum class Action {
    P1Up,
    P1Down,
    P1Left,
    P1Right,
    P2Up,
    P2Down,
    P2Left,
    P2Right,
    Pass,
    ToggleAi,
    Restart,
    Quit,
};

constexpr engine::Color kFloorColor{52, 56, 68};
constexpr engine::Color kWallColor{118, 98, 82};
constexpr int kAiPlayer = 1;       // the AI plays player 2
constexpr int kAiDelayTicks = 20;  // a third of a second, so its moves can be followed

// The window has one extra row of height, split into a strip above the board
// (moves left) and a strip below it (score).
constexpr float kStripHeight = kCellSize / 2.0f;

// cell_rect, moved down below the top strip.
engine::Rect board_rect(Cell cell, float inset = 0.0f) {
    engine::Rect rect = cell_rect(cell, inset);
    rect.y += kStripHeight;
    return rect;
}

constexpr engine::Color kNeutralBlockColor{86, 152, 172};
constexpr engine::Color kUsedMoveColor{36, 38, 46};
constexpr engine::Color kDrawColor{150, 150, 150};
constexpr engine::Color kMiddleTickColor{240, 240, 240};

// Per player: the piece itself, the blocks they own, and a dim tint for the
// floor they control.
constexpr engine::Color kPlayerColors[kPlayerCount] = {{236, 196, 84}, {214, 96, 150}};
constexpr engine::Color kOwnedBlockColors[kPlayerCount] = {{172, 136, 52}, {158, 64, 110}};
constexpr engine::Color kTerritoryColors[kPlayerCount] = {{92, 82, 56}, {88, 58, 80}};

struct Binding {
    Action action;
    Direction direction;
};

// One row per player. Each player only has their own keys, so a held key
// never moves the wrong piece when the turn changes.
constexpr Binding kMoves[kPlayerCount][4] = {
    {
        {Action::P1Up, Direction::Up},
        {Action::P1Down, Direction::Down},
        {Action::P1Left, Direction::Left},
        {Action::P1Right, Direction::Right},
    },
    {
        {Action::P2Up, Direction::Up},
        {Action::P2Down, Direction::Down},
        {Action::P2Left, Direction::Left},
        {Action::P2Right, Direction::Right},
    },
};

} // namespace

void DuelGame::setup(engine::Input& input) {
    using engine::Key;
    input.bind(Action::P1Up, Key::W);
    input.bind(Action::P1Down, Key::S);
    input.bind(Action::P1Left, Key::A);
    input.bind(Action::P1Right, Key::D);
    input.bind(Action::P2Up, Key::Up);
    input.bind(Action::P2Down, Key::Down);
    input.bind(Action::P2Left, Key::Left);
    input.bind(Action::P2Right, Key::Right);
    input.bind(Action::Pass, Key::Space);
    input.bind(Action::ToggleAi, Key::Tab);
    input.bind(Action::Restart, Key::R);
    input.bind(Action::Quit, Key::Escape);
}

void DuelGame::update(const engine::Input& input) {
    if (input.pressed(Action::Quit)) {
        quit_ = true;
        return;
    }
    if (input.just_pressed(Action::Restart)) {
        // Each new round the other player goes first.
        first_player_ = (first_player_ + 1) % kPlayerCount;
        match_ = with_first_player(start_, first_player_);
        ai_wait_ticks_ = 0;
        return;
    }
    if (input.just_pressed(Action::ToggleAi)) {
        ai_enabled_ = !ai_enabled_;
        std::printf("AI %s.\n", ai_enabled_ ? "on" : "off");
    }
    if (is_over(match_)) {
        return;
    }

    bool turn_taken = false;
    if (ai_enabled_ && match_.current == kAiPlayer) {
        // Human keys are ignored on the AI's turn.
        if (++ai_wait_ticks_ < kAiDelayTicks) {
            return;
        }
        ai_wait_ticks_ = 0;
        if (const auto direction = choose_move(match_)) {
            turn_taken = play_move(match_, *direction);
        } else {
            pass_turn(match_);
            turn_taken = true;
        }
    } else if (input.just_pressed(Action::Pass)) {
        // Not on key repeat: holding Space would pass for both players in turn.
        pass_turn(match_);
        turn_taken = true;
    } else {
        for (const auto& move : kMoves[match_.current]) {
            if (input.pressed(move.action)) {
                turn_taken = play_move(match_, move.direction);
                break;
            }
        }
    }

    if (turn_taken && is_over(match_)) {
        announce_result();
    }
}

void DuelGame::announce_result() const {
    std::printf("Player 1 controls %d cells, player 2 controls %d.\n", score(match_, 0),
                score(match_, 1));
    if (const auto winner = leader(match_)) {
        std::printf("Player %d wins!\n", *winner + 1);
    } else {
        std::printf("It's a draw.\n");
    }
}

void DuelGame::render(engine::Renderer& renderer) {
    const Grid& grid = match_.grid;

    for (int y = 0; y < grid.height(); ++y) {
        for (int x = 0; x < grid.width(); ++x) {
            const Cell cell{x, y};
            if (grid.at(cell) == Tile::Wall) {
                renderer.fill_rect(board_rect(cell), kWallColor);
                continue;
            }
            const auto owner = controller(match_, cell);
            renderer.fill_rect(board_rect(cell, 1.0f),
                               owner ? kTerritoryColors[*owner] : kFloorColor);
        }
    }
    for (std::size_t i = 0; i < match_.blocks.size(); ++i) {
        const int owner = match_.block_owners[i];
        renderer.fill_rect(board_rect(match_.blocks[i], 4.0f),
                           owner == kNoOwner ? kNeutralBlockColor : kOwnedBlockColors[owner]);
    }

    // The player whose turn it is is drawn bigger.
    for (int p = 0; p < kPlayerCount; ++p) {
        const bool active = !is_over(match_) && p == match_.current;
        renderer.fill_rect(board_rect(match_.players[p], active ? 5.0f : 12.0f), kPlayerColors[p]);
    }

    const float strip_w = static_cast<float>(grid.width() * kCellSize);

    // Top strip: one pip per move, coloured by whose move it will be and dark
    // once used. When the match is over it shows the winner's colour.
    const float pips_y = 5.0f;
    const float pips_h = kStripHeight - 10.0f;
    if (is_over(match_)) {
        const auto winner = leader(match_);
        renderer.fill_rect({8.0f, pips_y, strip_w - 16.0f, pips_h},
                           winner ? kPlayerColors[*winner] : kDrawColor);
    } else {
        const int total = start_.moves_left;
        const int used = total - match_.moves_left;
        const float pip_w = strip_w / static_cast<float>(total);
        for (int i = 0; i < total; ++i) {
            const engine::Color color =
                i < used ? kUsedMoveColor
                         : kPlayerColors[(first_player_ + i) % kPlayerCount];
            renderer.fill_rect({static_cast<float>(i) * pip_w + 2.0f, pips_y, pip_w - 4.0f, pips_h},
                               color);
        }
    }

    // Bottom strip: the score as a tug of war. Player 1 fills from the left
    // and player 2 from the right, in proportion to their scores; the white
    // tick marks the middle. Grey while nobody controls anything.
    const float bottom_y = kStripHeight + static_cast<float>(grid.height() * kCellSize);
    const engine::Rect score_bar{8.0f, bottom_y + 5.0f, strip_w - 16.0f, kStripHeight - 10.0f};
    const int first = score(match_, 0);
    const int second = score(match_, 1);
    if (first + second == 0) {
        renderer.fill_rect(score_bar, kUsedMoveColor);
    } else {
        const float first_w = score_bar.w * static_cast<float>(first) /
                              static_cast<float>(first + second);
        renderer.fill_rect({score_bar.x, score_bar.y, first_w, score_bar.h}, kPlayerColors[0]);
        renderer.fill_rect({score_bar.x + first_w, score_bar.y, score_bar.w - first_w, score_bar.h},
                           kPlayerColors[1]);
    }
    renderer.fill_rect({score_bar.x + score_bar.w / 2.0f - 1.0f, score_bar.y - 3.0f, 2.0f,
                        score_bar.h + 6.0f},
                       kMiddleTickColor);
}

} // namespace game
