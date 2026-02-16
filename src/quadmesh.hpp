#pragma once
#include <glad/glad.h>

class QuadMesh {
public:
    QuadMesh();
    ~QuadMesh();
    void draw() const;

private:
    GLuint vao, vbo;
};
