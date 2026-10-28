#pragma once

#include <utility>

#include <engine/app.h>

#include "match.h"

namespace game {

// Two players on one keyboard take turns pushing blocks to wall off space.
// Player 1 uses WASD, player 2 the arrow keys; Space passes the turn.
// By default the AI plays player 2; Tab switches between AI and human.
class DuelGame : public engine::Game {
public:
    explicit DuelGame(Match match) : start_(match), match_(std::move(match)) {}

    void setup(engine::Input& input) override;
    void update(const engine::Input& input) override;
    void render(engine::Renderer& renderer) override;
    bool wants_quit() const override { return quit_; }

private:
    void announce_result() const;

    Match start_;  // kept for restarting
    int first_player_ = 0;  // who started this round; R swaps it
    Match match_;
    bool ai_enabled_ = true;  // the AI plays player 2; Tab toggles it
    int ai_wait_ticks_ = 0;
    bool quit_ = false;
};

} // namespace game
