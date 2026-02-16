#include <glad/glad.h>
#include <SDL2/SDL.h>
#include "renderer.hpp"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static glm::mat4 projection;


// shaders hard-coded
static GLuint shaderProgram = 0;

static const char* vertexSrc = R"(
#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uModel;

void main()
{
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}
)";


static const char* fragmentSrc = R"(
#version 330 core

out vec4 FragColor;

uniform float uIntensity;

void main()
{
    vec3 base  = vec3(0.1);
    vec3 alert = vec3(0.2, 0.8, 1.0);

    vec3 color = mix(base, alert, uIntensity);
    FragColor = vec4(color, 1.0);
}
)";

static GLuint vao = 0;
static GLuint vbo = 0;
static float triangleVertices[] = {
     0.0f,  0.5f, 0.0f,   // topo
    -0.5f, -0.5f, 0.0f,   // esquerda
     0.5f, -0.5f, 0.0f    // direita
};

static GLuint compileShader(GLenum type, const char* src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[32];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader erro: " << log << std::endl;
    }
    return shader;
}


bool Renderer::init(SDL_Window* win)
{
    window = win;
    if (!window) return false;
    // Define OpenGL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);// 1 buffer desenha, outro espera
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);// tamanho do eixo Z

    glContext = SDL_GL_CreateContext(window);
    if (!glContext) return false;

    SDL_GL_MakeCurrent(window, glContext);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        return false;
    }

    SDL_GL_SetSwapInterval(1); // vsync
    glEnable(GL_DEPTH_TEST);// objetos à frente escondem os de trás

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(triangleVertices),
        triangleVertices,
        GL_STATIC_DRAW
    );

    // atributo posição (location = 0 no vertex shader)
    glVertexAttribPointer(
        0,              // location
        3,              // vec3
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);
    // limpeza de estado (boa prática)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    
    projection = glm::perspective(
        glm::radians(70.0f),      // FOV
        (float)w / (float)h,      // aspect
        0.1f,                     // near
        100.0f                    // far
    );


    return true;
}

void Renderer::clear()
{
    glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::present()
{
    SDL_GL_SwapWindow(window);
}

void Renderer::shutdown()
{
    if (glContext) {
        SDL_GL_DeleteContext(glContext);
        glContext = nullptr;
    }
    if (shaderProgram)
    {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
    }
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);

}

void Renderer::render(const Camera& camera, const Object3D& object, const CubeMesh& mesh)
{
    glUseProgram(shaderProgram);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    GLint viewLoc  = glGetUniformLocation(shaderProgram, "uView");
    GLint projLoc  = glGetUniformLocation(shaderProgram, "uProjection");

    glUniformMatrix4fv(
        modelLoc, 1, GL_FALSE, &object.getModelMatrix()[0][0]
    );

    glUniformMatrix4fv(
        viewLoc, 1, GL_FALSE, &camera.getViewMatrix()[0][0]
    );

    glUniformMatrix4fv(
        projLoc, 1, GL_FALSE, &projection[0][0]
    );

    GLint intenLoc = glGetUniformLocation(shaderProgram, "uIntensity");
    glUniform1f(intenLoc, 1.0f);


    mesh.draw();
}

void Renderer::renderTube(const Camera& camera,
                          const TubeGrid& tube,
                          const QuadMesh& quad)
{
    glUseProgram(shaderProgram);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    GLint viewLoc  = glGetUniformLocation(shaderProgram, "uView");
    GLint projLoc  = glGetUniformLocation(shaderProgram, "uProjection");
    GLint intenLoc = glGetUniformLocation(shaderProgram, "uIntensity");

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE,
        &camera.getViewMatrix()[0][0]);

    glUniformMatrix4fv(projLoc, 1, GL_FALSE,
        &projection[0][0]);

    for (const auto& cell : tube.getCells())
    {
        glm::mat4 model(1.0f);

        // posição
        model = glm::translate(model, cell.position);

        // Eixos locais do quad
        glm::vec3 normal = glm::normalize(cell.normal);
        glm::vec3 forward = glm::vec3(0, 0, -1); // eixo do tubo
        glm::vec3 right = glm::normalize(glm::cross(forward, normal));
        glm::vec3 up = glm::cross(normal, right);

        // Matriz de rotação explícita
        glm::mat4 rotation(1.0f);
        rotation[0] = glm::vec4(right, 0.0f);
        rotation[1] = glm::vec4(up,    0.0f);
        rotation[2] = glm::vec4(normal,0.0f);

        model *= rotation;

        // tamanho do quadrado
        model = glm::scale(model, glm::vec3(0.9f));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniform1f(intenLoc, cell.intensity);

        quad.draw();
    }
}

