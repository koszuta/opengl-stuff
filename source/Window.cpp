#include <stdio.h>
#include <string>

#include <SDL2/SDL.h>

#include "Window.hpp"

Window::Window(std::string title, int height, int width, bool resizable)
    : initialized_{false}, title_{title}, height_{height}, width_{width} {
    // Create SDL window
    int windowFlags = SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL;
    if (resizable) {
        windowFlags |= SDL_WINDOW_RESIZABLE;
    }
    this->window_ = SDL_CreateWindow(this->title_.c_str(), 0, 31, this->width_, this->height_, windowFlags);
    if (!this->window_) {
        fprintf(stderr, "Couldn't create SDL window: %s", SDL_GetError());
        return;
    }

    // Set window ID
    this->id_ = SDL_GetWindowID(this->window_);

    // Create GL Context
    this->context_ = SDL_GL_CreateContext(this->window_);
    if (!this->context_) {
        fprintf(stderr, "Couldn't create GL context: %s", SDL_GetError());
        return;
    }

    // Use OpenGL 4.6 core
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) != 0) {
        fprintf(stderr, "Couldn't set GL major version: %s\n", SDL_GetError());
        return;
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6) != 0) {
        fprintf(stderr, "Couldn't set GL minor version: %s\n", SDL_GetError());
        return;
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
        fprintf(stderr, "Couldn't set GL context profile: %s\n", SDL_GetError());
        return;
    }
    // Use double buffering
    if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
        fprintf(stderr, "Couldn't set GL double buffering: %s\n", SDL_GetError());
        return;
    }
    // Use 24-bit depth buffer
    if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) != 0) {
        fprintf(stderr, "Couldn't set GL depth size: %s\n", SDL_GetError());
        return;
    }

    // Use 4 samples for multisampling
    if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1) != 0) {
        fprintf(stderr, "Couldn't set GL multisample buffer count: %s\n", SDL_GetError());
        return;
    }
    if (SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4) != 0) {
        fprintf(stderr, "Couldn't set GL multisample sample count: %s\n", SDL_GetError());
        return;
    }

    // Turn on VSync (has to be done after a window context is created)
    if (SDL_GL_SetSwapInterval(-1) != 0) {
        printf("Couldn't set Adaptive VSync: %s\n", SDL_GetError());
        printf("Trying regular VSync instead...\n");
        if (SDL_GL_SetSwapInterval(1) != 0) {
            fprintf(stderr, "Couldn't set VSync: %s\n", SDL_GetError());
            return;
        }
    }

    this->initialized_ = true;
}

Window::~Window() {
    SDL_GL_DeleteContext(this->context_);
    SDL_DestroyWindow(this->window_);
}

void Window::rename(std::string title) {
    SDL_SetWindowTitle(this->window_, title.c_str());
}

bool Window::initialized() {
    return initialized_;
}

int Window::width() {
    return width_;
}

int Window::height() {
    return height_;
}

Uint32 Window::id() {
    return id_;
}

SDL_Window* Window::window() {
    return window_;
}

SDL_GLContext Window::context() {
    return context_;
}
