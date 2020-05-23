#ifndef INPUT_HPP
#define INPUT_HPP

#include <map>

#include <SDL2/SDL.h>

class Input {
public:
    static void init();

    static bool poll_events();

    // Keyboard events
    static bool key_is_down(SDL_Keycode key);
    static bool key_was_clicked(SDL_Keycode key);

    // Mouse events

    // Window events

private:
    // Keyboard events
    static bool keys_down_[SDL_NUM_SCANCODES];
    static bool keys_clicked_[SDL_NUM_SCANCODES];

    // Mouse events

    // Window events
    static std::map<Uint32, int> window_events_;
};

#endif