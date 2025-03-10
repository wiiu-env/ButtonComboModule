#pragma once

// Simple class to track buttons triggered and released; also perform suppression logic.

template<typename T>
struct ButtonTracker {
    void reset() noexcept {
        hold     = 0;
        trigger  = 0;
        release  = 0;
        suppress = 0;
    }

    void update(T buttons) noexcept {
        T changed = buttons ^ hold;
        hold      = buttons;
        trigger   = changed &  buttons;
        release   = changed & ~buttons;
    }

    void blockTriggered() noexcept {
        suppress |= trigger;
    }

    void unblockReleased() noexcept {
        suppress &= ~release;
    }

    template<typename U>
    void suppressButtons(U& buttons) {
        buttons &= ~suppress;
    }

private:
    T hold     = 0;
    T trigger  = 0;
    T release  = 0;
    T suppress = 0;
};
