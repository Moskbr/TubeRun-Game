#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


class Object3D {
    
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotationAxis;
    
    float rotationAngle;
    mutable glm::mat4 model;
    
    void updateModel();

public:
    Object3D();

    void setPosition(const glm::vec3 &pos);
    void setRotation(float angleRad, const glm::vec3 &axis);
    void setScale(const glm::vec3 &s);

    const glm::mat4& getModelMatrix() const;
    void setModelMatrix(const glm::mat4& m);
    const glm::vec3& getScale() const;
};