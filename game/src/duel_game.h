#pragma once

#include <utility>

#include <engine/app.h>

#include "match.h"

namespace game {

// Two players on one keyboard take turns pushing blocks to wall off space.
// Player 1 uses WASD, player 2 the arrow keys; Space passes the turn.
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
    Match match_;
    bool pass_was_held_ = false;
    bool quit_ = false;
};

} // namespace game
