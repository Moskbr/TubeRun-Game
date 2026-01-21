#include "game.hpp"
#include "renderer.hpp"
#include <iostream>

Game::Game() : running(false), window(nullptr), glContext(nullptr),
            moveFoward(0.0f), moveRight(0.0f), yaw(0.0f), pitch(0.0f)
{}

Game::~Game(){}

bool Game::init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "Erro SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("TubeRun",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            1280, 720,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if(!window) {
        std::cerr << "Erro janela: " << SDL_GetError() << std::endl;
        return false;
    }

    // Mouse relativo: deslocamento do cursor invisivel
    SDL_SetRelativeMouseMode(SDL_TRUE);
    if (!renderer.init(window)) {
        std::cerr << "Erro ao inicializar renderer\n";
        return false;
    }

    running = true;
    return true;
}


void Game::run()
{
    Uint64 lastCounter = SDL_GetPerformanceCounter();
    const float freq = (float)SDL_GetPerformanceFrequency();

    while (running)
    {
        Uint64 current = SDL_GetPerformanceCounter();
        float deltaTime = (current - lastCounter) / freq;
        lastCounter = current;

        processInput();
        update(deltaTime);
        render();
    }
}

void Game::processInput()
{
    moveFoward = 0.0f;
    moveRight  = 0.0f;

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            running = false;
        
        else if (event.type == SDL_MOUSEMOTION)
        {
            const float sensitivity = 0.1f;
            yaw   += event.motion.xrel * sensitivity;
            pitch -= event.motion.yrel * sensitivity;
            // limites
            if (pitch > 89.0f) pitch = 89.0f;
            else if (pitch < -89.0f) pitch = -89.0f;
        }    
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_W]) moveFoward += 1.0f;
    if (state[SDL_SCANCODE_S]) moveFoward -= 1.0f;
    if (state[SDL_SCANCODE_D]) moveRight += 1.0f;
    if (state[SDL_SCANCODE_A]) moveRight -= 1.0f;
}

void Game::update(float deltaTime)
{
    const float speed = 5.0f;

    float fowardMove = moveFoward * speed * deltaTime;
    float rightMove  = moveRight  * speed * deltaTime;

    camera.yaw = yaw;
    camera.pitch = pitch;
    camera.updateVectors();
    camera.moveForward(fowardMove);
    camera.moveRight(rightMove);
}

void Game::render()
{
    renderer.clear();
    renderer.render(camera);
    renderer.present();
}

void Game::shutdown()
{
    renderer.shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();
}