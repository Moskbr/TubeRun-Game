#pragma once
#include "camera.hpp"
#include <SDL2/SDL.h>
#include "object3d.hpp"
#include "cubemesh.hpp"
#include "tubegrid.hpp"
#include "quadmesh.hpp"

class Renderer {
    SDL_Window* window = nullptr;
    SDL_GLContext glContext = nullptr;
    public:
    bool init(SDL_Window* win);
    void clear();
    void present();
    void shutdown();
    void render(
        const Camera& camera,
        const Object3D& object,
        const CubeMesh& mesh
    );
    void renderTube(const Camera& camera,
                const TubeGrid& tube,
                const QuadMesh& quad
    );

};
