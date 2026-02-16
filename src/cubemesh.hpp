#pragma once
#include <glad/glad.h>

class CubeMesh {
    
    GLuint vao;
    GLuint vbo;

public:
    CubeMesh();
    ~CubeMesh();

    void draw() const;
};