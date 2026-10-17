#pragma once

#include <array>
#include <cstddef>
#include <type_traits>
#include <vector>

namespace engine {

// Physical keys the engine knows about. Add keys here when a game needs them.
enum class Key {
    Up,
    Down,
    Left,
    Right,
    W,
    A,
    S,
    D,
    Escape,
    Count
};

// Key repeat, measured in fixed update ticks.
struct RepeatSettings {
    int delay_ticks = 15;    // ticks held before the first repeat
    int interval_ticks = 7;  // ticks between later repeats
};

// Maps keys to game-defined actions and tracks them once per fixed update.
//
// Games define their own action enum and bind keys to it, for example:
//     enum class Action { MoveUp, Quit };
//     input.bind(Action::MoveUp, Key::Up);
//     if (input.pressed(Action::MoveUp)) { ... }
class Input {
public:
    using ActionId = std::size_t;

    void bind(ActionId action, Key key);
    void set_repeat(RepeatSettings settings) { repeat_ = settings; }

    // True on the tick the action starts, and again on each key repeat.
    bool pressed(ActionId action) const;
    // True on every tick while any key bound to the action is down.
    bool held(ActionId action) const;

    template <typename E>
        requires std::is_enum_v<E>
    void bind(E action, Key key) { bind(static_cast<ActionId>(action), key); }

    template <typename E>
        requires std::is_enum_v<E>
    bool pressed(E action) const { return pressed(static_cast<ActionId>(action)); }

    template <typename E>
        requires std::is_enum_v<E>
    bool held(E action) const { return held(static_cast<ActionId>(action)); }

    // Called by the engine's main loop; games don't need these.
    void on_key_down(Key key);
    void on_key_up(Key key);
    void release_all();
    void tick();

private:
    static constexpr std::size_t kKeyCount = static_cast<std::size_t>(Key::Count);

    struct ActionState {
        std::vector<Key> keys;
        int held_ticks = -1;  // -1 while not held, 0 on the first tick
        bool pressed = false;
    };

    ActionState& state_for(ActionId action);

    std::vector<ActionState> actions_;
    std::array<bool, kKeyCount> key_down_{};
    // Set on key down and cleared after each tick, so a press and release
    // that both happen between two ticks is still seen.
    std::array<bool, kKeyCount> key_tapped_{};
    RepeatSettings repeat_;
};

} // namespace engine
