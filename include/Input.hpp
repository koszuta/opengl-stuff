#ifndef INPUT_HPP
#define INPUT_HPP

#include <map>

#include <SDL2/SDL.h>

class Input {
public:
    static void init();

    static bool poll_events();
    static bool should_quit();

    // Keyboard events
    static bool key_is_down(SDL_Keycode key);

    // Mouse events

    // Window events

private:
    static bool should_quit_;

    // Keyboard events
    static bool keys_[SDL_NUM_SCANCODES];

    // Mouse events

    // Window events
    static std::map<Uint32, int> window_events_;
};

#endif