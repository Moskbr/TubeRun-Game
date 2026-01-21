#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;
    mutable glm::mat4 viewMatrix;

    public:
    float yaw, pitch;
    glm::vec3 position;

    Camera();
    void updateVectors();
    void moveForward(float amount);
    void moveRight(float amount);
    const glm::mat4& getViewMatrix() const;
};