#include <doctest/doctest.h>

#include <engine/app.h>

namespace {

// Fails the test if the engine ever gets far enough to call into the game.
class UnreachableGame : public engine::Game {
public:
    void setup(engine::Input&) override { FAIL("setup should not be called"); }
    void update(const engine::Input&) override { FAIL("update should not be called"); }
    void render(engine::Renderer&) override { FAIL("render should not be called"); }
};

} // namespace

TEST_CASE("run rejects a nonpositive update rate before opening a window") {
    UnreachableGame game;
    engine::AppConfig config;

    for (int rate : {0, -1, -60}) {
        CAPTURE(rate);
        config.updates_per_second = rate;
        CHECK(engine::run(config, game) != 0);
    }
}
