#include <stdio.h>
#include <map>

#include <SDL2/SDL.h>

#include "Input.hpp"

bool Input::keys_down_[];
bool Input::keys_clicked_[];
std::map<Uint32, int> Input::window_events_ = std::map<Uint32, int>();

bool Input::key_is_down(SDL_Keycode k) {
    return Input::keys_down_[SDL_GetScancodeFromKey(k)];
}

bool Input::key_was_clicked(SDL_Keycode k) {
    SDL_Scancode sc = SDL_GetScancodeFromKey(k);
    bool clicked = Input::keys_clicked_[sc];
    Input::keys_clicked_[sc] = false;
    if (clicked) {
        // printf("%s was clicked...\n", SDL_GetKeyName(k));
    }
    return clicked;
}

bool Input::poll_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT: {
                // printf("X clicked...\n");
                return false;
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
                SDL_Scancode sc = e.key.keysym.scancode;
                if (!Input::keys_down_[sc]) {
                    Input::keys_down_[sc] = true;
                    // printf("%s pressed...\n", SDL_GetKeyName(SDL_GetKeyFromScancode(sc)));
                }
            } break;
            case SDL_KEYUP: {
                SDL_Scancode sc = e.key.keysym.scancode;
                if (Input::keys_down_[sc]) {
                    Input::keys_down_[sc] = false;
                    Input::keys_clicked_[sc] = true;
                    // printf("%s clicked...\n", SDL_GetKeyName(SDL_GetKeyFromScancode(sc)));
                }
            } break;
        }
    }
    return true;
}
