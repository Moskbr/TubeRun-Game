#include "game.hpp"
#include "renderer.hpp"
#include <iostream>
#include "object3d.hpp"
#include "cubemesh.hpp"

Object3D cubeObj;
TubeGrid tube{8, 40, 5.0f};
std::unique_ptr<QuadMesh> quad;
TubeCell cell_obs;
glm::vec3 obsPos;

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

    cube = std::make_unique<CubeMesh>();
    quad = std::make_unique<QuadMesh>();

    cubeObj.setPosition({0.0f, 0.0f, -6.0f});
    cubeObj.setScale({1.0f, 1.0f, 1.0f});

    const int laneCount = tube.getLanesCount();

    std::vector<TubeCell> cells = tube.getCells();
    int index = rand() % cells.size();
    cell_obs = cells[index];
    obsPos = cell_obs.position - cell_obs.normal * 0.5f;// ??

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
        else if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE)
                running = false;
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
    float speed = 5.0f;
    float acceleration = 1.5f;
    float maxSpeed = 40.0f;
    /*
    float fowardMove = moveFoward * speed * deltaTime;
    float rightMove  = moveRight  * speed * deltaTime;*/
    camera.yaw = yaw;
    camera.pitch = pitch;
    camera.updateVectors();
    //camera.moveForward(fowardMove);
    //camera.moveRight(rightMove);
    speed += acceleration * deltaTime;
    speed = speed > maxSpeed ? maxSpeed: speed;
    camera.position.z -= speed * deltaTime;

    // obstáculo vem em direção ao jogador
    obstacle.z += speed * deltaTime;
    // atualiza tubo com base no obstáculo
    tube.update(camera.position.z, obstacle);

    // --- Colisão câmera x obstáculo (detecção apenas) ---
    const float tubeRadius = 5.0f;        // o mesmo usado no TubeGrid
    const float obstacleRadius = 0.6f;    // aproximação do cubo

    glm::vec3 playerPos = camera.position;

    cubeObj.setPosition(obsPos);
    //cubeObj.setScale({0.6f, 0.6f, 0.6f});

    // ----- Rotação do cubo para que o vetor normal seja o mesmo da célula
    glm::vec3 localNormal(0, 0, 1);// eixo z do cubo
    glm::vec3 target = glm::normalize(cell_obs.normal);// vetor normal objetivo

    // produto vetorial para encontrar eixo de rotação
    glm::vec3 axis = glm::cross(localNormal, target);
    // produto escalar: n1.n2 = |n1|.|n2|.cos(theta) --> n1.n2 = cos(theta)
    float dot = glm::dot(localNormal, target);
    dot = glm::clamp(dot, -1.0f, 1.0f);
    // extrai o angulo entre os vetores normais com arccosseno
    float angle = acos(dot);

    if (glm::length(axis) < 0.0001f)
        axis = glm::vec3(0, 1, 0);
    else
        axis = glm::normalize(axis);


    float dist = glm::distance(playerPos, obsPos);

    if (dist < obstacleRadius)
    {
        // por enquanto, apenas log
        std::cout << "Colisão detectada!\n";
    }


}

void Game::render()
{
    renderer.clear();

    renderer.renderTube(camera, tube, *quad);

    renderer.render(camera, cubeObj, *cube);

    renderer.present();
}



void Game::shutdown()
{
    renderer.shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();
}