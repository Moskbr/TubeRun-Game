#pragma once
#include "camera.hpp"
#include <SDL2/SDL.h>

class Renderer {
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    public:
    bool init(SDL_Window* win);
    void clear();
    void present();
    void shutdown();
    void render(const Camera& camera);
};
