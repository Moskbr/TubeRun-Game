#include "cubemesh.hpp"

static float cubeVertices[] = {
    // frente
    -0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,

    // trás
    -0.5f,-0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,

    // esquerda
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f,-0.5f,
    -0.5f,-0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,

    // direita
     0.5f, 0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f, 0.5f,-0.5f,
     0.5f, 0.5f, 0.5f,

    // topo
    -0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f, 0.5f, 0.5f,
     0.5f, 0.5f,-0.5f,
    -0.5f, 0.5f,-0.5f,

    // base
    -0.5f,-0.5f,-0.5f,
     0.5f,-0.5f,-0.5f,
     0.5f,-0.5f, 0.5f,
     0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f, 0.5f,
    -0.5f,-0.5f,-0.5f
};

CubeMesh::CubeMesh()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(cubeVertices),
        cubeVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,
                        3 * sizeof(float), (void*)0);
    
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

CubeMesh::~CubeMesh()
{
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void CubeMesh::draw() const
{
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}