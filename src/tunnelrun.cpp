#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Shader.h"

#include <iostream>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;// lighting
};

struct Player {
    float t = 0.0f;  // progress in spline
    float speed = 0.0f;
    float maxSpeed = 10.0f;
    float acceleration = 1.5f;
    float angle = 0.0f;   // angular position in tunnel
    float angularSpeed = 2.5f;
};

glm::vec3 catmullRom (glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * ((2.0f*p1) +
                  t  * (-p0 + p2) +
                  t2 * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3) + 
                  t3 * (-p0 + 3.0f*p1 - 3.0f*p2 + p3)
                );
}
// first derivative
glm::vec3 catmullRomTangent (glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
    float t2 = t * t;

    return 0.5f * ((-p0 + p2) +
                   2.0f*t * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3) + 
                   3.0f*t2 * (-p0 + 3.0f*p1 - 3.0f*p2 + p3)
                );
}
// Calcula a trajetória (pontos centrais - centers[]) intermediários entre P1 e P2, tendo P0 e P3 como
// pontos de controle que dão a direção da inclinação.
// 'samplesPerSegment' é a resolução da curva entre P1 e P2, dada a entrada cp[] = {P0,P1,P2,P3}
// Caso entrada cp[] = {P0,P1,P2,P3,P4}, o laço 'i' calcula o segmento P1->P2 e depois P2->P3 ...
void sampleSpline (const std::vector<glm::vec3>& cp, int samplesPerSegment,
                    std::vector<glm::vec3>& centers, std::vector<glm::vec3>& tangents)
{
    int count = cp.size();
    if (count < 4) return;
    centers.clear();
    tangents.clear();

    for (int i=0; i < count - 3; i++) {
        for (int j=0; j < samplesPerSegment; j++) {
            
            float t = (float)j / (float)samplesPerSegment;

            glm::vec3 pos = catmullRom(cp[i], cp[i+1], cp[i+2], cp[i+3], t);
            glm::vec3 rawTan = catmullRomTangent(cp[i], cp[i+1], cp[i+2], cp[i+3], t);
            glm::vec3 tan = (glm::length(rawTan) > 0.0001f) ? glm::normalize(rawTan) : glm::vec3(0.0f,0.0f,0.0f);
            
            centers.push_back(pos);
            tangents.push_back(tan);
        }

        if (i == count - 4) {
            glm::vec3 pos    = catmullRom(cp[i], cp[i+1], cp[i+2], cp[i+3], 1.0f);
            glm::vec3 rawTan = catmullRomTangent(cp[i], cp[i+1], cp[i+2], cp[i+3], 1.0f);
            glm::vec3 tan    = (glm::length(rawTan) > 0.0001f)
                                   ? glm::normalize(rawTan) : glm::vec3(0,0,1);
            centers.push_back(pos);
            tangents.push_back(tan);
        }
    }
}

// Parallel Transform Frames
void computeFrames (const std::vector<glm::vec3>& tangents, std::vector<glm::quat>& Q,
                        std::vector<glm::vec3>& N, std::vector<glm::vec3>& B)
{
    int n = tangents.size();
    Q.resize(n);
    N.resize(n);
    B.resize(n);

    glm::vec3 T0 = tangents[0];

    glm::vec3 up = glm::vec3(0,1,0);
    if (fabs(glm::dot(T0, up)) > 0.99f)// T0 is nearly parallel to 'up'
        up = glm::vec3(1,0,0);// probably not used
    
    B[0] = glm::normalize(glm::cross(T0, up));
    N[0] = glm::normalize(glm::cross(B[0], T0));

    Q[0] = glm::quat(1, 0, 0, 0); // identity

    for (int i=1; i < n; i++)
    {
        glm::vec3 Tprev = glm::normalize(tangents[i-1]);
        glm::vec3 Tcurr = glm::normalize(tangents[i]);
        
        float dot = glm::dot(Tprev, Tcurr);
        glm::quat dq;

        if (dot > 0.999f) {// tangents are nearly parallel, keep the same normal and binormal
            dq = glm::quat(1, 0, 0, 0);
        }
        else if (dot < -0.999f) {// if they are on opposite directions, choose axis rotation (direction of y or x)
            glm::vec3 axis = glm::normalize(glm::cross(Tprev, glm::vec3(0,1,0)));
            if (glm::length(axis) < 1e-5f)
                axis = glm::normalize(glm::cross(Tprev, glm::vec3(1,0,0)));
            
            dq = glm::angleAxis(glm::pi<float>(), axis); // force 180 degree quaternion
        }
        else {
            //glm::vec3 v = glm::cross(tangents[i-1], tangents[i]);
            //v = glm::normalize(v);// T[i-1] * T[i] = ||T[i-1]|| ||T[i]|| cos(theta), but ||T[i-1]|| = ||T[i]|| = 1
            //float angle = acos(glm::clamp(glm::dot(tangents[i-1], tangents[i]), -1.0f, 1.0f));
            /*
            // Formula de Rotação de Rodrigues: v_rot = v*cos(theta) + (k X v)*sin(theta) + k(k*v)(1-cos(theta)), k: eixo de rotação
            glm::mat4 R = glm::rotate(glm::mat4(1.0f), angle, v); // rotation matrix to align T[i-1] to T[i]
            N[i] = glm::normalize(glm::vec3(R * glm::vec4(N[i-1], 0.0f))); // rotate previus normal to get current normal
            B[i] = glm::normalize(glm::cross(tangents[i], N[i]));*/

            dq = glm::rotation(Tprev, Tcurr); // obtain the quaternion for the minimum rotation from Tprev to Tcurr
        }
        Q[i] = glm::normalize(dq * Q[i-1]); // sum this rotation to the "memory of the curve", for smoothness
        N[i] = glm::normalize(Q[i] * N[i-1]); // apply the result on the initial normal and binormal
        B[i] = glm::normalize(Q[i] * B[i-1]);
    }
}

// The thunnel is a sequence of circles (rings) along the negative Z axis, with a resolution determined by the number os segments (sides)
// and its angle, equal for all slices. All rings has its own central point (vector centers[]) determining the trajectory of the tunnel.
void generateTunnel (float radius, int segments, const std::vector<glm::vec3>& centers,
        const std::vector<glm::quat>& Q, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
        std::vector<glm::vec3>& N, std::vector<glm::vec3>& B)
{
    vertices.clear();
    indices.clear();

    for (int i=0; i < centers.size(); i++) {
        for (int j=0; j < segments; j++) {

            float theta = (float)j / (float)segments * 2.0f * glm::pi<float>();

            glm::vec3 p=centers[i]+radius*(N[i]*(float)cos(theta)+B[i]*(float)sin(theta));

            Vertex v;
            v.position = p;
            v.normal = glm::normalize(centers[i] - p);// points to the center
            v.uv = glm::vec2((float)j/(float)segments, (float)i/(float)centers.size());

            vertices.push_back(v);
        }
    }
        for (int i=0; i < centers.size() - 1; i++)
            for (int j=0; j < segments; j++) {
                // supposing counter-clockwise
                int current = i * segments + j; // bottom point (A)
                int upper = current + segments;  // point just above (B)
                int nextSeg = (j + 1) % segments; // just set the next segment
                int left = i * segments + nextSeg; // left point from the bottom (C)
                int upperLeft = left + segments;   // point above the left one (D)
                /* So there we have our square:
                    D----B
                    |  / |
                    | /  |
                    C----A
                    and also the triangles needed to make it.
                */

                // Triangle: ABC
                indices.push_back(current);
                indices.push_back(upper);
                indices.push_back(left);

                // Triangle: CBD
                indices.push_back(left);
                indices.push_back(upper);
                indices.push_back(upperLeft);
        }
}

glm::mat4 computePlayerCamera (float t, float playerAngle, float radius, std::vector<glm::vec3>& centers,
                                std::vector<glm::vec3>& tangents, std::vector<glm::quat>& Q, glm::vec3& cameraPosition,
                                std::vector<glm::vec3>& N, std::vector<glm::vec3>& B)
{
    int i = (int)t;
    int next = std::min(i + 1, (int)centers.size() - 1);
    float localT = t - i;

    // linear interpolation (LERP) -> mix(), for camera transitions over center points
    glm::vec3 center = glm::mix(centers[i], centers[next], localT);
    glm::vec3 forward = glm::normalize(glm::mix(tangents[i], tangents[next], localT));
    // spherical linear interpolation (SLERP) for camera rotation over Q list -> more smoothness
    //glm::quat q = glm::normalize(glm::slerp(Q[i], Q[next], localT));

    // local base coords, with rotation applied over the initial state
    glm::vec3 Nf = glm::normalize(glm::mix(N[i], N[next], localT)); // supposing N0 = direction of y axis
    glm::vec3 Bf = glm::normalize(glm::mix(B[i], B[next], localT)); // supposing B0 = direction of x axis

    // camera position at the inner wall of the tunnel
    float offsetGround = 0.2f;
    float x = cos(playerAngle) * (radius - offsetGround);
    float y = sin(playerAngle) * (radius - offsetGround);
    glm::vec3 position = center + Nf*x + Bf*y;
    cameraPosition = position; // global variable for now

    // update the up vector to align with the normal of the surface
    glm::vec3 up = glm::normalize(-(Nf * (float)cos(playerAngle) + Bf * (float)sin(playerAngle)));

    // return looAt matrix with a farer target -> look ahead, to remove jitter
    glm::vec3 target = position + forward * 1.0f;
    return glm::lookAt(position, target, up);
}

void updatePlayer (Player& player, float deltaTime)
{
    player.speed += player.acceleration * deltaTime; // progressive acceleration
    if (player.speed > player.maxSpeed) player.speed = player.maxSpeed;
    player.t += player.speed * deltaTime; // advances on spline
}

// ---------------------------------- GAME CODE ------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, Player& player, float deltaTime);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// mouse capture
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);// enable depth in Z-Buffer
    glDisable(GL_CULL_FACE);
    
    // Build Shader
    Shader shader("tunnel.vs", "tunnel.fs");

    // Spline
    std::vector<glm::vec3> controlPoints = {
        //{0,0,0}, {0,0,10}, {5,0,20}, {5,5,30}, {0,10,40}, {-5,5,50}, {0,0,60}
        {0,0,0}, {0,0,15}, {8,0,30}, {8,6,50}, {0,10,70}, {-8,6,90}, {0,0,110}
    };
    std::vector<glm::vec3> centers, tangents;
    sampleSpline(controlPoints, 40, centers, tangents); // samplesPerSegment = 20 -> 40

    // Compute Quaternion
    std::vector<glm::quat> Q;
    std::vector<glm::vec3> N, B;
    computeFrames(tangents, Q, N, B);

    // Make Tunnel
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    float radius = 2.0f;
    int segments = 32;
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    generateTunnel(radius, segments, centers, Q, vertices, indices, N, B);

    // OpenGL Buffers
    unsigned int VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // pass the vertices data to GPU
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // GL_STATIC_DRAW indicates that data is modified once and drawn many times -> VRAM
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    // set up indices as element buffer, the order of the vertices to be drawn
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // position: layout=0, vec3, float, normalized already, stride = for each Vertex, starting from the first
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // normal: layout=1, vec3, float, normalized already, stride = for each Vertex, starting from the first offset
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);
    // uv: layout=2, vec2, float, normalized already, stride = for each Vertex, starting from the first offset
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    //  -------------- Game Loop --------------------
    // as projection rarely changes, no need to pass it in each frame
    shader.use(); // activate first
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader.setMat4("projection", projection);

    Player player;
    glm::vec3 cameraPos;
    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        processInput(window, player, deltaTime);
        // player movement
        updatePlayer(player, deltaTime);
        if (player.t >= centers.size() - 1) player.t = 0.0f; // Temporary: restart******
        // camera view
        glm::mat4 view = computePlayerCamera(player.t,player.angle,radius,centers,tangents,Q,cameraPos,N,B);
        
        // render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // send uniforms
        shader.setMat4("view", view);
        // obtacle models (in process) ********************
        glm::mat4 model = glm::mat4(1.0f); // temporary
        shader.setMat4("model", model);
        // fragment shader uniforms:
        shader.setVec3("lightPos", cameraPos);
        shader.setVec3("viewPos", cameraPos);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: delete vaos and vbos

    glfwTerminate();
    return 0;
}

// process all input
void processInput(GLFWwindow *window, Player& player, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    float input = 0.0f;
    float distance = 2.0f;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        input += distance;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        input -= distance;
    
    // input only modifies angle position inside the tunnel
    player.angle += input * player.angularSpeed * deltaTime;
    player.angle *= 0.999f;
    // keep inside interval [0, 2pi] -> transform in positive degree angle if necessary
    const float TWO_PI = 6.283185307f;
    if (player.angle > TWO_PI) player.angle -= TWO_PI;
    if (player.angle < 0.0f) player.angle += TWO_PI;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}