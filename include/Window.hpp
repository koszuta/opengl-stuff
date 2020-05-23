#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>

#include <SDL2/SDL.h>

class Window {
public:
    Window(std::string title, int height, int width, bool resizable);
    ~Window();

    void rename(std::string title);

    bool initialized();
    int width();
    int height();
    Uint32 id();
    SDL_Window* window();
    SDL_GLContext context();

private:
    bool initialized_;
    std::string title_;
    int height_, width_;
    Uint32 id_;
    SDL_Window* window_;
    SDL_GLContext context_;
};

#endif