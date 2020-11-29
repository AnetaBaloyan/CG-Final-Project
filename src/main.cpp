#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float yaw = -90.0f, pitch = 0.0f;
bool firstMouse = true;
float lastX = 400, lastY = 300;
float fov = 70.0f;
bool gameView = false;
float ambientStrength = 0.4f, diffuseStrength = 1.0f, specularStrength = 0.5f;

//-------- CYLINDER PARAMETERS.
float radius = 0.5f, bottom = -0.5, height = 1.2f;
int sectors = 500, strips = 200;
float shininess = 32.0f;

glm::vec3 cameraPos    = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraFront  = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp     = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 up           = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraFront));

glm::vec3 lightPos(radius * 0.8, bottom + height * 0.9, radius * 1.7);
//glm::vec3 lightColor(1.0f, 1.0f, 1.0f); // white light
//glm::vec3 lightColor(0.941f, 0.878f, 0.274f); // yellow light
glm::vec3 lightColor(1.0f, 0.960f, 0.933f); // pale light
glm::vec3 objectColor(0.819f, 0.066f, 0.254f);
glm::vec4 backgroundColor(0.137f, 0.168f, 0.168f, 0.0f);


class Shader {
    public:
        // the program ID
        unsigned int ID;

        // constructor reads and builds the shader
        Shader(const char* vertexPath, const char* fragmentPath) {
            // 1. retrieve the vertex/fragment source code from filePath
            std::string vertexCode;
            std::string fragmentCode;
            std::ifstream vShaderFile;
            std::ifstream fShaderFile;
            // ensure ifstream objects can throw exceptions:
            vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                // open files
                vShaderFile.open(vertexPath);
                fShaderFile.open(fragmentPath);
                std::stringstream vShaderStream, fShaderStream;
                // read file's buffer contents into streams
                vShaderStream << vShaderFile.rdbuf();
                fShaderStream << fShaderFile.rdbuf();
                // close file handlers
                vShaderFile.close();
                fShaderFile.close();
                // convert stream into string
                vertexCode = vShaderStream.str();
                fragmentCode = fShaderStream.str();
            }
            catch (std::ifstream::failure e) {
                std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            }
            const char *vShaderCode = vertexCode.c_str();
            const char *fShaderCode = fragmentCode.c_str();

            // 2. compile shaders
            unsigned int vertex, fragment;
            int success;
            char infoLog[512];

            // vertex Shader
            vertex = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertex, 1, &vShaderCode, NULL);
            glCompileShader(vertex);
            // print compile errors if any
            glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(vertex, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            };

            // Fragment Shader
            fragment = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragment, 1, &fShaderCode, NULL);
            glCompileShader(fragment);
            // print compile errors if any
            glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(fragment, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            };

            // shader Program
            ID = glCreateProgram();
            glAttachShader(ID, vertex);
            glAttachShader(ID, fragment);
            glLinkProgram(ID);
            // print linking errors if any
            glGetProgramiv(ID, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(ID, 512, NULL, infoLog);
                std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            }

            // delete the shaders as they're linked into our program now and no longer necessary
            glDeleteShader(vertex);
            glDeleteShader(fragment);

        }

        // use/activate the shader
        void use() {
            glUseProgram(ID);
        }

        // utility uniform functions
        void setBool(const std::string &name, bool value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
        }
        void setInt(const std::string &name, int value) const
        {
            glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
        }
        void setFloat(const std::string &name, float value) const
        {
            glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
        }
        void setVec3(const std::string &name, glm::vec3 value) const
        {
            glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
        }
        void setMat4(const std::string &name, glm::mat4 value) const
        {
            glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
        }
};
#endif

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;

        if(!gameView) {
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

    if(gameView) {
        yaw += xoffset;
        pitch += yoffset;
    } else {
        yaw -= xoffset;
        pitch -= yoffset;
    }

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    if(gameView) {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(direction);
    } else {
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

void generateFan(float r, float height, int sectors, glm::vec3 normal, unsigned int* VAO, int id, int id_norm, bool clockwise) {
    std::vector<glm::vec3> vertices;

    // Add the center.
    glm::vec3 center(0.0f, height, 0.0f);
    vertices.emplace_back(center);

    // Add the normal.
    vertices.emplace_back(normal);
    float angle = 360.0f / (float)sectors;

    for(int i = 0; i < sectors; i++) {
        glm::vec3 point;
        float curAngle;

        if(clockwise) {
            curAngle = glm::radians((float)i * -1 * angle);
        } else {
            curAngle = glm::radians((float)i * angle);
        }

        point.y = center.y;
        point.x = cos(curAngle) * r;
        point.z = sin(curAngle) * r;

        // Add the point.
        vertices.emplace_back(point);

        // Add the normal.
        vertices.emplace_back(normal);
    }

    // Add the first point again to close the cycle.
    vertices.emplace_back(vertices[2]);
    vertices.emplace_back(normal);

    unsigned int VBO;
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(id);

    glVertexAttribPointer(id_norm, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(id_norm);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void generateStrip(float r, float bottom, float top, int sectors, unsigned int* VAO, int id, int id_norm) {
    std::vector<glm::vec3> vertices;

    float angle = 360.0f / (float)sectors;
    float step = height / (float)strips;

    for(int k = 0; k < strips; k++) {
        glm::vec3 firstPoint, secondPoint;
        glm::vec3 firstNormal, secondNormal;

        for(int i = 0; i < sectors; i++) {
            float strip_top = bottom + ((float)k + 1.0f) * step;
            float strip_bottom = bottom + (float)k * step;

            glm::vec3 point1, point2, point3, normal;

            float curAngle = glm::radians((float)i * angle);
            float nextAngle = glm::radians((float)(i + 1) * angle);

            point1.y = strip_bottom;
            point1.x = cos(curAngle) * r;
            point1.z = sin(curAngle) * r;

            point2.y = strip_top;
            point2.x = cos(curAngle) * r;
            point2.z = sin(curAngle) * r;

            point3.y = strip_bottom;
            point3.x = cos(nextAngle) * r;
            point3.z = sin(nextAngle) * r;

            normal = glm::normalize(glm::cross(point1 - point3, point2 - point3));

            // Add the points and the normals.
            vertices.emplace_back(point1);
            vertices.emplace_back(normal);

            vertices.emplace_back(point2);
            vertices.emplace_back(normal);

            if(i == 0) {
                firstPoint = point1;
                firstNormal = normal;

                secondPoint = point2;
                secondNormal = normal;
            }
        }

        // Add the first two points again to close the cycle.
        // Make sure to preserve the order.
        vertices.emplace_back(firstPoint);
        vertices.emplace_back(firstNormal);

        vertices.emplace_back(secondPoint);
        vertices.emplace_back(secondNormal);
    }


    unsigned int VBO;
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(*VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(id);

    glVertexAttribPointer(id_norm, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));
    glEnableVertexAttribArray(id_norm);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CG HW 03", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_CULL_FACE); // Make only front faces visible.
    glEnable(GL_DEPTH_TEST); // Enable depth testing.

//-------- Top Shader.
    Shader topShader("../resources/shaders/top_vertex.shader", "../resources/shaders/fragment.shader");
    topShader.use();
    topShader.setVec3("objectColor", objectColor);
    topShader.setVec3("lightColor",  lightColor);
    topShader.setVec3("lightPos", lightPos);
    topShader.setVec3("viewPos", cameraPos);
    topShader.setFloat("shininess", shininess);
    topShader.setFloat("ambientStrength", ambientStrength);
    topShader.setFloat("diffuseStrength", diffuseStrength);
    topShader.setFloat("specularStrength", specularStrength);

    glm::mat4 model = glm::mat4(1.0f);
    topShader.setMat4("model", model);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

//-------- Bottom Shader.
    Shader bottomShader("../resources/shaders/bottom_vertex.shader", "../resources/shaders/fragment.shader");
    bottomShader.use();
    bottomShader.setVec3("objectColor", objectColor);
    bottomShader.setVec3("lightColor", lightColor);
    bottomShader.setVec3("lightPos", lightPos);
    bottomShader.setVec3("viewPos", cameraPos);
    bottomShader.setFloat("shininess", shininess);
    bottomShader.setFloat("ambientStrength", ambientStrength);
    bottomShader.setFloat("diffuseStrength", diffuseStrength);
    bottomShader.setFloat("specularStrength", specularStrength);

    model = glm::mat4(1.0f);
    bottomShader.setMat4("model", model);

//-------- Body Shader.
    Shader bodyShader("../resources/shaders/body_vertex.shader", "../resources/shaders/fragment.shader");
    bodyShader.use();
    bodyShader.setVec3("objectColor", objectColor);
    bodyShader.setVec3("lightColor",  lightColor);
    bodyShader.setVec3("lightPos", lightPos);
    bodyShader.setVec3("viewPos", cameraPos);
    bodyShader.setFloat("shininess", shininess);
    bodyShader.setFloat("ambientStrength", ambientStrength);
    bodyShader.setFloat("diffuseStrength", diffuseStrength);
    bodyShader.setFloat("specularStrength", specularStrength);

    model = glm::mat4(1.0f);
    bodyShader.setMat4("model", model);

//-------- Lighting Shader.
    Shader lightShader("../resources/shaders/light_vertex.shader", "../resources/shaders/light_fragment.shader");
    lightShader.use();
    lightShader.setVec3("lightPos", lightPos);
    lightShader.setVec3("lightColor",  lightColor);


    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.07f));
    lightShader.setMat4("model", model);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    // Cube.
    float vertices[] = {
            -0.5f, 0.5f, 0.5f,    // top front left
            0.5f, 0.5f, 0.5f,   // top front right
            0.5f, 0.5f, -0.5f,     // top back right
            -0.5f, 0.5f, -0.5f,     // top back left
            -0.5f, -0.5f, 0.5f,     // bottom front left
            0.5f, -0.5f, 0.5f,    // bottom front right
            0.5f, -0.5f, -0.5f,     // bottom back right
            -0.5f, -0.5f, -0.5f,     // bottom back left
    };

    // Cube.
    unsigned int indices[] = {  // note that we start from 0!
            0, 1, 2,
            0, 2, 3,
            7, 3, 2,
            7, 2, 6,
            5, 6, 2,
            5, 2, 1,
            4, 5, 1,
            4, 1, 0,
            4, 0, 3,
            4, 3, 7,
            4, 7, 6,
            4, 6, 5
    };

    unsigned int TOP_VAO, BOTTOM_VAO, BODY_VAO;

    glm::vec3 topNormal(0.0f, 1.0f, 0.0f);
    generateFan(radius, bottom + height, sectors, topNormal, &TOP_VAO, 2, 3, true);

    glm::vec3 bottomNormal(0.0f, -1.0f, 0.0f);
    generateFan(radius, bottom, sectors, bottomNormal, &BOTTOM_VAO, 4, 5, false);

    generateStrip(radius, bottom, bottom + height, sectors, &BODY_VAO, 6, 7);

    unsigned int VBO, VAO, EBO, lightVAO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        cameraRight = glm::normalize(glm::cross(up, cameraFront));


        // render
        // ------
        glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth buffer.

        // DRAW!!!
        if(gameView) {
            view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        } else {
            view = glm::lookAt(cameraPos, cameraFront, cameraUp);
        }
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        // Draw top.
        topShader.use();

        topShader.setMat4("view", view);
        topShader.setMat4("projection", projection);

        glBindVertexArray(TOP_VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sectors + 2);
        glBindVertexArray(0);

        // Draw bottom.
        bottomShader.use();

        bottomShader.setMat4("view", view);
        bottomShader.setMat4("projection", projection);

        glBindVertexArray(BOTTOM_VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, sectors + 2);
        glBindVertexArray(0);

        // Draw body.
        bodyShader.use();

        bodyShader.setMat4("view", view);
        bodyShader.setMat4("projection", projection);

        glBindVertexArray(BODY_VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, strips * (3 * sectors + 2));
        glBindVertexArray(0);

        // Draw the light cube.
        lightShader.use();

        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);

        glBindVertexArray(lightVAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &TOP_VAO);
    glDeleteBuffers(1, &BOTTOM_VAO);
    glDeleteBuffers(1, &BODY_VAO);
    glDeleteBuffers(1, &lightVAO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    float cameraSpeed = 2.5f * deltaTime;
    if(gameView) {
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
            cameraPos    = glm::vec3(0.0f, 0.0f,  3.0f);
            cameraFront  = glm::vec3(0.0f, 0.0f, 0.0f);
            cameraUp     = glm::vec3(0.0f, 1.0f,  0.0f);
            cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    } else {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { //GAME VIEW.
            gameView = true;
            yaw = -90.0f;
            pitch = 0.0f;
            cameraPos    = glm::vec3(0.0f, 0.0f,  3.0f);
            cameraFront  = glm::vec3(0.0f, 0.0f, -1.0f);
            cameraUp     = glm::vec3(0.0f, 1.0f,  0.0f);
            cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        }
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}