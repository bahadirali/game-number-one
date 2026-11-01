#pragma once

#include <string>
#include <utility>
#include <vector>

#include <engine/app.h>

#include "match.h"

namespace game {

// Two players on one keyboard take turns pushing blocks to wall off space.
// Player 1 uses WASD, player 2 the arrow keys; Space passes the turn.
// By default the AI plays player 2; Tab switches between AI and human.
// Against the AI, player 1 can use either WASD or the arrow keys.
// The levels are played in order: when a round is over, Enter moves on to
// the next one, and after the last it starts again from the first.
class DuelGame : public engine::Game {
public:
    // `levels` must not be empty, and all levels must be the same size,
    // since the window is sized for the first one.
    explicit DuelGame(std::vector<Match> levels)
        : levels_(std::move(levels)), match_(levels_.front()) {}

    void setup(engine::Input& input) override;
    void update(const engine::Input& input) override;
    void render(engine::Renderer& renderer) override;
    bool wants_quit() const override { return quit_; }

private:
    void start_round(std::size_t level);
    void announce_result() const;
    void show_message(std::string text, int player);
    void draw_notice(engine::Renderer& renderer, const std::string& text,
                     engine::Color color) const;

    std::vector<Match> levels_;  // starting positions, kept for restarting
    std::size_t level_ = 0;      // index into levels_ of the level being played
    int first_player_ = 0;       // who started this round; every new round swaps it
    Match match_;
    bool ai_enabled_ = true;  // the AI plays player 2; Tab toggles it
    int ai_wait_ticks_ = 0;

    // A short notice drawn over the board until message_ticks_ runs out.
    std::string message_;
    int message_player_ = 0;  // whose colour the notice is drawn in
    int message_ticks_ = 0;
    bool quit_ = false;
};

} // namespace game
