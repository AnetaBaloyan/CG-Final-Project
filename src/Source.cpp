#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"

using namespace std;


float windowWidth = 800;
float windowHeight = 600;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;
float lastX = 400, lastY = 300;
float fov = 70.0f;
bool gameView = false;


glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraFront));

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;

        if (!gameView) {
            cameraFront.z = 0.0f;
            yaw = 0.0f;
        }
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    if (gameView) {
        yaw += xoffset;
        pitch += yoffset;
    }
    else {
        yaw -= xoffset;
        pitch -= yoffset;
    }

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    if (gameView) {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    }
    else {
        float radius = glm::length(cameraPos);
        glm::vec3 position;
        position.x = sin(glm::radians(90 - pitch)) * sin(glm::radians(yaw)) * radius;
        position.y = cos(glm::radians(90 - pitch)) * radius;
        position.z = cos(glm::radians(yaw)) * sin(glm::radians(90 - pitch)) * radius;
        cameraPos = position;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 120.0f)
        fov = 120.0f;
}

void processInput(GLFWwindow* window)
{
    float cameraSpeed = 2.5f * deltaTime;
    if (gameView) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { // ROTATE VIEW.
            gameView = false;
            pitch = 0.0f;
            yaw = 0.0f;
            cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, 0.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }
    else {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { //GAME VIEW.
            gameView = true;
            yaw = -90.0f;
            pitch = 0.0f;
            cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
            cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
            cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        //popup_menu();
    }
}

struct Surface {
    GLfloat* coordinates; //array holding the vertex information.
    int size; // the generated coordinates array size.
    GLuint* indexBuffer; // array holding the indices of the triangle strips
    int indexCount;
} water, terrain;

void GenerateIndexedTriangleStripPlane(Surface &surface, int hVertices, int vVertices, int stride, float wLen, float hLen, float xMin, float zMin) {
    GLfloat* coordinates = new GLfloat[hVertices * vVertices * stride];

    float x;
    float z;

    for (int i = 0; i < vVertices; i++) {
        z = zMin + i * hLen;

        for (int j = 0; j < hVertices; j++) {
            float x = (float)xMin + j * (float)wLen;
            int index = (i * hVertices + j) * stride;
            printf("\nInd: %d, %d, %d", (i * hVertices + j), i, j);

            // vertex coord
            coordinates[index] = x;
            coordinates[index + 1] = 0.;
            coordinates[index + 2] = z;

            // texture u, v
            float u = (float)j / (float)(hVertices - 1);
            float v = (float)i / (float)(vVertices - 1);
            coordinates[index + 3] = u;
            coordinates[index + 4] = 1 - v;
        }
    }

    for (int i = 0; i < hVertices * vVertices * stride; i++) {
        if (i % 5 == 0) {
            printf("\n");
        }
        printf("\n %f", coordinates[i]);
    }

    int indices_length = hVertices * (vVertices - 2) * 2 + hVertices * 2 + (vVertices - 2) * 2;
    GLuint* indices = new GLuint[indices_length];
    printf("\nIndex Length: %d", indices_length);

    printf("\nIndices");

    int index = 0;
    for (int i = 0; i < vVertices - 1; i++) {
        for (int j = 0; j < hVertices; j++) {
            indices[index++] = i * hVertices + j;
            if (j == 0 && i > 0) {
                indices[index++] = indices[index - 1];
            }
            indices[index++] = i * hVertices + j + hVertices;
            if (j == (hVertices - 1)) {
                indices[index++] = indices[index - 1];
            }
        }
    }
    printf("\n");

    for (int i = 0; i < indices_length; i++) {
        printf("%u, ", indices[i]);
    }
    printf("\n");

    surface.coordinates = coordinates;
    surface.size = hVertices * vVertices * stride;
    surface.indexBuffer = indices;
    surface.indexCount = indices_length;
}



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    //// TODO
    //glfwSetMouseButtonCallback(window, mouse_button_callback);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSwapInterval(1);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // call glewInit after creating the context...
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }

    // plane coordinates setup
    glm::vec3 A = glm::vec3(-1., 0., -1.);
    glm::vec3 B = glm::vec3(1., 0., -1.);
    glm::vec3 C = glm::vec3(1., 0., 1.);
    glm::vec3 D = glm::vec3(-1., 0., 1.);

    int hVertices = 5;
    int vVertices = 5;
    float w_len = (B.x - A.x) / (float)(hVertices - 1);
    float h_len = (D.z - A.z) / (float)(vVertices - 1);

    // water plane
    GenerateIndexedTriangleStripPlane(water, hVertices, vVertices, 5, w_len, h_len, A.x, A.z);
    Mesh waterMesh = Mesh(water.coordinates, water.size);
    waterMesh.BindIndexBuffer(water.indexBuffer, water.indexCount);
    waterMesh.AddLayout(3); // vertex layout
    waterMesh.AddLayout(2); // texture layout

    // terrain plane
    //GenerateIndexedTriangleStripPlane(terrain, hVertices, vVertices, 5, w_len, h_len, A.x, A.z);
    //Mesh terrainMesh = Mesh(terrain.coordinates, terrain.size);
    //terrainMesh.BindIndexBuffer(terrain.indexBuffer, terrain.indexCount);
    //terrainMesh.AddLayout(3); // vertex layout
    //terrainMesh.AddLayout(2); // texture layout


    // SHADER
    Shader* shader = new Shader("./resources/shaders/vertex.shader", "", "./resources/shaders/fragment.shader");
    unsigned int program = shader->GetProgramId();
    shader->bind();
    // END SHADER

    // TEXTURE
    Texture waterTexture = Texture("./resources/images/WaterDiffuse.png");

    glm::mat4 model = glm::mat4(1.0f);
    shader->SetMat4("model", model);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);// process keyboard inputs
        cameraRight = glm::normalize(glm::cross(up, cameraFront));

        // DRAW!!!
        if (gameView) {
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        }
        else {
            view = glm::lookAt(cameraPos, cameraFront, cameraUp);
        }
        projection = glm::perspective(glm::radians(fov), windowWidth / windowHeight, 0.1f, 100.0f);

        shader->SetMat4("view", view);
        shader->SetMat4("projection", projection);

        // elapsed time
        float time = (float)glfwGetTime();
        shader->SetFloat("elapsedTime", time);

        // draw water
        waterTexture.Bind();
        waterMesh.DrawElements();

        //Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glDeleteProgram(program);

    glfwTerminate();
    return 0;
}