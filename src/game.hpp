#pragma once
#include <SDL2/SDL.h>
#include "camera.hpp"
#include "renderer.hpp"
#include <memory>
#include "cubemesh.hpp"
#include "obstacle.hpp"


class Game {
    bool running;
    SDL_Window* window;
    SDL_GLContext glContext;// para 3D com OpenGL

    // Input FPS
    float moveFoward, moveRight; // movimentacao
    float yaw, pitch;// rotacao do mouse

    Camera camera;
    Renderer renderer;
    Obstacle obstacle;
    std::unique_ptr<CubeMesh> cube;
    glm::mat4 obstacleModel;

    void processInput();
    void update(float deltaTime);
    void render();
public:
    Game();
    ~Game();
    bool init();
    void run();
    void shutdown();
};