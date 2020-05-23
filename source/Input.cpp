#include <stdio.h>
#include <map>

#include <SDL2/SDL.h>

#include "Input.hpp"

bool Input::should_quit_ = false;
bool Input::keys_[];
std::map<Uint32, int> Input::window_events_ = std::map<Uint32, int>();

bool Input::should_quit() {
    return Input::should_quit_;
}

bool Input::key_is_down(SDL_Keycode k) {
    return Input::keys_[SDL_GetScancodeFromKey(k)];
}

bool Input::poll_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_QUIT: {
            printf("X clicked...\n");
            Input::should_quit_ = true;
        } break;
        // case SDL_WINDOWEVENT:
        //     if (e.window.windowID == windowID) {
        //         if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
        //             int newWidth  = e.window.data1;
        //             int newHeight = e.window.data2;
        //             printf("Window resized to %dx%d, aspect %f...\n", newWidth, newHeight, (float) newWidth / (float) newHeight);
        //             if((float) newWidth / (float) newHeight > aspect) {
        //                 newHeight = (1.0f / aspect) * newWidth;
        //             }
        //             else {
        //                 newWidth = aspect * newHeight;
        //             }
        //             printf("Window resized to %dx%d, aspect %f...\n", newWidth, newHeight, (float) newWidth / (float) newHeight);
        //             // windowWidth  = newWidth;
        //             // windowHeight = newHeight;
        //             // SDL_SetWindowSize(gWindow, windowWidth, windowHeight);
        //         }
        //     }
        //     break;
        case SDL_KEYDOWN: {
            SDL_Scancode k = e.key.keysym.scancode;
            if (!Input::keys_[k]) {
                Input::keys_[k] = true;
            }
            printf("%x pressed...\n", k);
        } break;
        case SDL_KEYUP: {
            SDL_Scancode k = e.key.keysym.scancode;
            if (Input::keys_[k]) {
                Input::keys_[k] = false;
            }
            printf("%x released...\n", k);
        } break;
        }
    }
    return true;
}
