#include "object3d.hpp"

Object3D::Object3D()
    :   position(0.0f),
        scale(1.0f),
        rotationAxis(0.0f,1.0f,0.0f),
        rotationAngle(0.0f),
        model(1.0f)
{
    updateModel();
}

void Object3D::setPosition(const glm::vec3 &pos)
{
    position = pos;
    updateModel();
}

void Object3D::setRotation(float angleRad, const glm::vec3& axis)
{
    rotationAngle = angleRad;
    rotationAxis = axis;
    updateModel();
}

void Object3D::setScale(const glm::vec3& s)
{
    scale = s;
    updateModel();
}

void Object3D::updateModel()
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotationAngle, rotationAxis);
    model = glm::scale(model, scale);
}

const glm::mat4& Object3D::getModelMatrix() const
{
    return model;
}

void Object3D::setModelMatrix(const glm::mat4& m)
{
    model = m;
}

const glm::vec3& Object3D::getScale() const
{
    return scale;
}
