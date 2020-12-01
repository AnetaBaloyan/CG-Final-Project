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

using namespace std;

float windowWidth = 800;
float windowHeight = 600;

// camera
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 View = glm::mat4(1.0f);
Camera *camera = new Camera(cameraPosition, cameraUp);

bool firstMouse = true;
float lastX = windowWidth / 2.0;
float lastY = windowHeight / 2.0;

void processInput(GLFWwindow *window) {
    Camera_Movement movement = NONE;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        movement = FORWARD;
    } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        movement = BACKWARD;
    } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        movement = LEFT;
    } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        movement = RIGHT;
    }
    camera->ProcessKeyboard(movement, 1.);
}


void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos * 1.0;
        lastY = ypos * 1.0;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

int main(void) {
    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    //process mouse inputs
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glfwSwapInterval(1);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // call glewInit after creating the context...
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }


    glm::vec3 A = glm::vec3(-1., 0., -1.);
    glm::vec3 B = glm::vec3(1., 0., -1.);
    glm::vec3 C = glm::vec3(1., 0., 1.);
    glm::vec3 D = glm::vec3(-1., 0., 1.);


    int n_width = 3;
    int n_height = 3;
    int dim = 3;

    GLfloat *coordinates = new GLfloat[n_width * n_height * dim];

    float w_len = (B.x - A.x) / (float) (n_width - 1);
    float h_len = (D.z - A.z) / (float) (n_height - 1);

    float x;
    float z;
    for (int i = 0; i < n_height; i++) {
        z = A.z + i * h_len;

        for (int j = 0; j < n_width; j++) {
            float x = (float) A.x + j * (float) w_len;
            int index = (i * n_width + j) * dim;
            printf("\nInd: %d, %d, %d", i * n_height + j, i, j);

            coordinates[index] = x;
            coordinates[index + 1] = 0;
            coordinates[index + 2] = z;

            // normal
            /*coordinates[index + 3] = 0;
            coordinates[index + 4] = 1;
            coordinates[index + 3] = 0;*/
        }
    }

    for (int i = 0; i < n_width * n_height * dim; i++) {
        if (i % 3 == 0) {
            printf("\n");
        }
        printf("\n %f", coordinates[i]);
    }

    int indices_length = n_width * (n_height - 2) * 2 + n_width * 2 + (n_height - 2) * 2;
    GLuint *indices = new GLuint[indices_length];
    printf("\nIndex Length: %d", indices_length);

    printf("\nIndices");

    int index = 0;
    for (int i = 0; i < n_height - 1; i++) {
        for (int j = 0; j < n_width; j++) {
            indices[index++] = i * n_width + j;
            if (j == 0 && i > 0) {
                indices[index++] = indices[index - 1];
            }
            indices[index++] = i * n_width + j + n_width;
            if (j == (n_width - 1)) {
                indices[index++] = indices[index - 1];
            }
        }
    }
    printf("\n");

    for (int i = 0; i < indices_length; i++) {
        printf("%u, ", indices[i]);
    }
    printf("\n");

    GLuint position_buffer;
    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, n_width * n_height * dim * sizeof(float), coordinates, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);
    // normal attribute
    /*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);*/

    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_length * sizeof(GLuint), indices, GL_STATIC_DRAW);

    Shader *shader = new Shader(".shaders/vertex.shader", "", "./shaders/fragment.shader");
    unsigned int program = shader->GetProgramId();
    shader->bind();

    glm::mat4 Model = glm::mat4(1.0f);
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
                                            (float) windowWidth / (float) windowHeight, 0.1f, 100.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);// process keyboard inputs

        glm::mat4 View = camera->GetViewMatrix();
        shader->SetMat4("view", View);
        shader->SetMat4("model", Model);
        shader->SetMat4("projection", Projection);

        glDrawElements(GL_TRIANGLE_STRIP, indices_length, GL_UNSIGNED_INT, nullptr);

        //Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glDeleteProgram(program);

    glfwTerminate();
    return 0;
}