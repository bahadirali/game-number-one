#include "engine/input.h"

#include <algorithm>

namespace engine {

namespace {

std::size_t index_of(Key key) {
    return static_cast<std::size_t>(key);
}

} // namespace

Input::ActionState& Input::state_for(ActionId action) {
    if (action >= actions_.size()) {
        actions_.resize(action + 1);
    }
    return actions_[action];
}

void Input::bind(ActionId action, Key key) {
    auto& keys = state_for(action).keys;
    if (std::find(keys.begin(), keys.end(), key) == keys.end()) {
        keys.push_back(key);
    }
}

bool Input::pressed(ActionId action) const {
    return action < actions_.size() && actions_[action].pressed;
}

bool Input::just_pressed(ActionId action) const {
    return action < actions_.size() && actions_[action].held_ticks == 0;
}

bool Input::held(ActionId action) const {
    return action < actions_.size() && actions_[action].held_ticks >= 0;
}

void Input::on_key_down(Key key) {
    key_down_[index_of(key)] = true;
    key_tapped_[index_of(key)] = true;
}

void Input::on_key_up(Key key) {
    key_down_[index_of(key)] = false;
}

void Input::release_all() {
    key_down_.fill(false);
    // Drop pending taps too, so a key pressed just before focus was lost
    // doesn't still fire on the next tick.
    key_tapped_.fill(false);
}

void Input::tick() {
    for (auto& action : actions_) {
        bool down = false;
        bool tapped = false;
        for (Key key : action.keys) {
            down = down || key_down_[index_of(key)];
            tapped = tapped || key_tapped_[index_of(key)];
        }

        if (tapped) {
            // A fresh key press always restarts the action, even if another
            // bound key was already held.
            action.held_ticks = 0;
        } else if (down) {
            ++action.held_ticks;
        } else {
            action.held_ticks = -1;
        }

        const int t = action.held_ticks;
        const bool is_repeat = t >= repeat_.delay_ticks && repeat_.interval_ticks > 0 &&
                               (t - repeat_.delay_ticks) % repeat_.interval_ticks == 0;
        action.pressed = t == 0 || is_repeat;
    }
    key_tapped_.fill(false);
}

} // namespace engine
