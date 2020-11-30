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

// camera
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 View = glm::mat4(1.0f);

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
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


	glm::vec3 A = glm::vec3(-1., 0., -1.);
	glm::vec3 B = glm::vec3(1., 0., -1.);
	glm::vec3 C = glm::vec3(1., 0., 1.);
	glm::vec3 D = glm::vec3(-1., 0., 1.);

	int n_width = 3;
	int n_height = 3;
	int dim = 3;

	GLfloat* coordinates = new GLfloat[n_width * n_height * dim];

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
	GLuint* indices = new GLuint[indices_length];
	printf("\nIndex Length: %d", indices_length);

	printf("\nIndices");

	int index = 0;
	for (int i = 0; i < n_height-1; i++) {
		for (int j = 0; j < n_width; j++) {
			indices[index++] = i * n_width + j;
			if (j == 0 && i > 0) {
				indices[index++] = indices[index-1];
			}
			indices[index++] = i * n_width + j + n_width;
			if (j == (n_width - 1)) {
				indices[index++] = indices[index-1];
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (n_width * n_height) * sizeof(GLuint), indices, GL_STATIC_DRAW);

	Shader* shader = new Shader("C:/Users/Nina/source/repos/Project/Project/shaders/vertex.shader", "", 
		"C:/Users/Nina/source/repos/Project/Project/shaders/fragment.shader");

	unsigned int program = shader->GetProgramId();
	shader->bind();

	Camera camera = Camera(cameraPosition, cameraUp);

	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 View = camera.GetViewMatrix();
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f),
		(float)800.0 / (float)600.0, 0.1f, 100.0f);

	shader->SetMat4("model", Model);
	shader->SetMat4("projection", Projection);
	shader->SetMat4("view", View);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// Render here 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawElements(GL_TRIANGLE_STRIP, n_width * n_height, GL_UNSIGNED_INT, nullptr);

		//Swap front and back buffers 
		glfwSwapBuffers(window);	

		// Poll for and process events 
		glfwPollEvents();
	}

	glDeleteProgram(program);

	glfwTerminate();
	return 0;
}