#pragma once
#include <glm/glm.hpp>

struct Obstacle {
    float angle;   // posição angular
    float z;       // posição longitudinal
    float radius;  // tamanho de colisão
public:
    int lane;
    glm::vec3 color;
};

