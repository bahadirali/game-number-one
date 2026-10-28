#include <doctest/doctest.h>

#include <engine/input.h>

using engine::Input;
using engine::Key;

namespace {

enum class Action { Move, Other };

Input make_input() {
    Input input;
    input.set_repeat({.delay_ticks = 3, .interval_ticks = 2});
    input.bind(Action::Move, Key::Right);
    input.bind(Action::Move, Key::D);
    input.bind(Action::Other, Key::Escape);
    return input;
}

} // namespace

TEST_CASE("a held key fires once, then repeats after the delay") {
    Input input = make_input();
    input.on_key_down(Key::Right);

    // Ticks 0..7: fire on 0, then on the delay (3) and every 2 ticks after.
    const bool expected[] = {true, false, false, true, false, true, false, true};
    for (bool fires : expected) {
        input.tick();
        CHECK(input.pressed(Action::Move) == fires);
        CHECK(input.held(Action::Move));
    }
    CHECK_FALSE(input.pressed(Action::Other));
}

TEST_CASE("releasing a key stops the action") {
    Input input = make_input();
    input.on_key_down(Key::Right);
    input.tick();
    input.on_key_up(Key::Right);
    input.tick();
    CHECK_FALSE(input.pressed(Action::Move));
    CHECK_FALSE(input.held(Action::Move));
}

TEST_CASE("a tap between two ticks still counts") {
    Input input = make_input();
    input.on_key_down(Key::D);
    input.on_key_up(Key::D);
    input.tick();
    CHECK(input.pressed(Action::Move));
    input.tick();
    CHECK_FALSE(input.pressed(Action::Move));
}

TEST_CASE("losing focus cancels a press that hasn't been ticked yet") {
    Input input = make_input();
    input.on_key_down(Key::Right);
    input.release_all();
    input.tick();
    CHECK_FALSE(input.pressed(Action::Move));
    CHECK_FALSE(input.held(Action::Move));
}

TEST_CASE("unbound actions are never pressed") {
    Input input;
    input.tick();
    CHECK_FALSE(input.pressed(Action::Move));
    CHECK_FALSE(input.held(Action::Move));
}

TEST_CASE("just_pressed fires once per press and ignores key repeat") {
    Input input = make_input();
    input.on_key_down(Key::Right);
    input.tick();
    CHECK(input.just_pressed(Action::Move));
    for (int i = 0; i < 10; ++i) {  // well past the repeat delay
        input.tick();
        CHECK_FALSE(input.just_pressed(Action::Move));
    }

    input.on_key_up(Key::Right);
    input.tick();
    input.on_key_down(Key::Right);
    input.tick();
    CHECK(input.just_pressed(Action::Move));
}

TEST_CASE("just_pressed sees a tap between two ticks") {
    Input input = make_input();
    input.on_key_down(Key::D);
    input.on_key_up(Key::D);
    input.tick();
    CHECK(input.just_pressed(Action::Move));
}
