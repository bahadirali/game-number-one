#pragma once

#include <utility>

#include <engine/app.h>

#include "layout.h"
#include "level.h"

namespace game {

class PuzzleGame : public engine::Game {
public:
    explicit PuzzleGame(Level level) : level_(std::move(level)) {}

    void setup(engine::Input& input) override;
    void update(const engine::Input& input) override;
    void render(engine::Renderer& renderer) override;
    bool wants_quit() const override { return quit_; }

private:
    Level level_;
    bool quit_ = false;
};

} // namespace game
