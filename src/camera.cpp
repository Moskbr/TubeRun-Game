#include "camera.hpp"
#include <cmath>

Camera::Camera() : position(0.0f,0.0f,3.0f),yaw(-90.0f),pitch(0.0f)
{
    updateVectors();
}

void Camera::updateVectors()
{
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = glm::sin(glm::radians(yaw))*cos(glm::radians(pitch));

    forward = glm::normalize(dir);
    right = glm::normalize(glm::cross(forward, glm::vec3(0,1,0)));
    up = glm::normalize(glm::cross(right, forward));
}

void Camera::moveForward(float amount)
{
    position += forward * amount;
}

void Camera::moveRight(float amount)
{
    position += right * amount;
}

const glm::mat4& Camera::getViewMatrix() const
{
    viewMatrix = glm::lookAt(position, position+forward, up);
    return viewMatrix;
}