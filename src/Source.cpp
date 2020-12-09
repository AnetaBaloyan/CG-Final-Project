#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "Mesh.h"
#include "Ray.h"
#include "Intersection.h"

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

// plane coordinates setup
glm::vec3 A = glm::vec3(-1., 0.3, -1.);
glm::vec3 B = glm::vec3(1., 0.3, -1.);
glm::vec3 C = glm::vec3(1., 0.3, 1.);
glm::vec3 D = glm::vec3(-1., 0.3, 1.);

Camera camera = Camera();

glm::vec3 ripplePoint;
float startTime = -10000;

//void mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//	if (firstMouse) {
//		lastX = xpos * 1.0;
//		lastY = ypos * 1.0;
//		firstMouse = false;
//	}
//	
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos;
//	lastX = xpos;
//	lastY = ypos;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	float offset = deltaTime * 100;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.ProcessMouseMovement(offset, 0);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.ProcessMouseMovement(-offset, 0);
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.ProcessMouseMovement(0, -offset);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.ProcessMouseMovement(0, offset);
}

Ray ConstructRayThroughPixel(int i, int j)
{
	float imageAspectRatio = windowWidth / windowHeight;
	float tanFOVHalf = tan(camera.Zoom * 2);

	float pixelCamera_x = (2 * (i + 0.5) / (float)windowWidth - 1) * imageAspectRatio * tanFOVHalf;
	float pixelCamera_y = (1 - 2 * (j + 0.5) / (float)windowHeight) * tanFOVHalf;

	glm::vec4 vec = glm::vec4(glm::normalize(glm::vec3(pixelCamera_x, pixelCamera_y, -1)), 1);
	glm::vec4 transformed = vec * camera.GetInvertedCamera();

	return Ray(camera.Position, glm::vec3(transformed.x, 0.3, transformed.z));
}

bool Intersect(const Ray& r, glm::vec3& P) {
	glm::vec3 N = glm::vec3(0.0, 1.0, 0.0);

	printf("Ray org %f %f %f", r.orig[0], r.orig[1], r.orig[2]);
	printf("\nRay dir %f %f %f", r.dir[0], r.dir[1], r.dir[2]);

	float dotNRay = glm::dot(N, r.dir);

	printf("\nDotNRay %f", dotNRay);

	if (dotNRay <= 0) {
		return false;
	}

	float D_vec = glm::dot(N, A);

	float t = -(D_vec - glm::dot(N, r.orig)) / dotNRay;

	//float t = (glm::dot(N, r.orig) + D_vec) / dotNRay;

	printf("\nT %f", t);

	if (t < 0) {
		return false;
	}

	P = r.orig + t * glm::normalize(r.dir);
	printf("\nP coord %f %f %f", P.x, P.y, P.z);
	return true;
	/*glm::vec3 edge1 = B - A;
	glm::vec3 edge2 = C - B;
	glm::vec3 edge3 = D - C;
	glm::vec3 edge4 = A - D;

	glm::vec3 C1 = P - A;
	glm::vec3 C2 = P - B;
	glm::vec3 C3 = P - C;
	glm::vec3 C4 = P - D;

	return glm::dot(N, glm::cross(edge1, C1)) > 0
		&& glm::dot(N, glm::cross(edge2, C2)) > 0
		&& glm::dot(N, glm::cross(edge3, C3)) > 0
		&& glm::dot(N, glm::cross(edge4, C4)) > 0;*/
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);
		printf("\n");
		cout << "Cursor Position at " << xpos << " : " << ypos << endl;

		Ray ray = ConstructRayThroughPixel(xpos, ypos);
		glm::vec3 P;
		bool intersect = Intersect(ray, P);

		if (intersect) {
			ripplePoint = P;
			startTime = (float)glfwGetTime();
			printf("\n");
			cout << "Ripple Position at " << P.x << " : " << P.y << " : " << P.z << endl;
		}
	}
}


struct Surface {
	GLfloat* coordinates; //array holding the vertex information.
	int size; // the generated coordinates array size.
	GLuint* indexBuffer; // array holding the indices of the triangle strips
	int indexCount;
} water, terrain;


void GenerateIndexedTriangleStripPlane(Surface &surface, int hVertices, int vVertices, int stride, float wLen, float hLen, float xMin, float zMin, float y) {
	GLfloat* coordinates = new GLfloat[hVertices * vVertices * stride];

	float x;
	float z;

	for (int i = 0; i < vVertices; i++) {
		z = zMin + i * hLen;

		for (int j = 0; j < hVertices; j++) {
			float x = (float)xMin + j * (float)wLen;
			int index = (i * hVertices + j) * stride;
			//printf("\nInd: %d, %d, %d", (i * hVertices + j), i, j);

			// vertex coord
			coordinates[index] = x;
			coordinates[index + 1] = y;
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
			//printf("\n");
		}
		//printf("\n %f", coordinates[i]);
	}

	int indices_length = hVertices * (vVertices - 2) * 2 + hVertices * 2 + (vVertices - 2) * 2;
	GLuint* indices = new GLuint[indices_length];

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

	surface.coordinates = coordinates;
	surface.size = hVertices * vVertices * stride;
	surface.indexBuffer = indices;
	surface.indexCount = indices_length;
}


int main(void)
{
	GLFWwindow* window;

	vec4 backgroundColor = vec4(0.8, 0.9, 0.87, 1);

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
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetCursorPosCallback(window, mouse_callback);
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

	int hVertices = 500;
	int vVertices = 500;
	float w_len = (B.x - A.x) / (float)(hVertices - 1);
	float h_len = (D.z - A.z) / (float)(vVertices - 1);

	// WATER PART
	GenerateIndexedTriangleStripPlane(water, hVertices, vVertices, 5, w_len, h_len, A.x, A.z, 0.3);
	Mesh waterMesh = Mesh(water.coordinates, water.size);
	waterMesh.BindIndexBuffer(water.indexBuffer, water.indexCount);
	waterMesh.AddLayout(3); // vertex layout
	waterMesh.AddLayout(2); // texture layout

	Shader* waterShader = new Shader("./resources/shaders/water_vertex.shader", 
		"./resources/shaders/water_geometry.shader", "./resources/shaders/water_fragment.shader");
	waterShader->bind();
	glm::mat4 model = glm::mat4(1.0f);
	waterShader->SetMat4("model", model);

	// TERRAIN PART
	GenerateIndexedTriangleStripPlane(terrain, hVertices, vVertices, 5, w_len, h_len, A.x, A.z, 0);
	Mesh terrainMesh = Mesh(terrain.coordinates, terrain.size);
	terrainMesh.BindIndexBuffer(terrain.indexBuffer, terrain.indexCount);
	terrainMesh.AddLayout(3); // vertex layout
	terrainMesh.AddLayout(2); // texture layout
	

	Shader* terrainShader = new Shader("./resources/shaders/terrain_vertex.shader", 
	"./resources/shaders/terrain_geometry.shader", "./resources/shaders/terrain_fragment.shader");
	terrainShader->bind();
	terrainShader->SetMat4("model", model);

	// GRASS PART
	Shader* grassShader = new Shader("./resources/shaders/terrain_vertex.shader",
		"./resources/shaders/grass_geometry.shader", "./resources/shaders/grass_fragment.shader");
	grassShader->bind();
	grassShader->SetMat4("model", model);

	// TEXTURE
	Texture waterTexture = Texture("./resources/images/WaterDiffuse.png");
	Texture terrainTexture = Texture("./resources/images/TerrainDiffuse.png");
	Texture heightTexture = Texture("./resources/images/TerrainHeightMap.png");
	Texture grassDistTexture = Texture("./resources/images/GrassDistribution.png");
	Texture grassDiffTexture = Texture("./resources/images/GrassDiffuse.png");


	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// Render here 
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_CULL_FACE);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);// process keyboard inputs

		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(fov), windowWidth / windowHeight, 0.1f, 100.0f);
		
		// elapsed time
		float time = (float)glfwGetTime();

		// draw terrain
		terrainShader->bind();
		terrainShader->SetMat4("view", view);
		terrainShader->SetMat4("projection", projection);
		terrainShader->SetVec3("lightDir", glm::vec3(-1.0, -1.0, -1.0));
		terrainShader->SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		terrainShader->SetFloat("ambientStrength", 0.3);
		terrainShader->SetFloat("diffuseStrength", 0.7);

		int tshId = terrainShader->GetProgramId();
		int i1 = glGetUniformLocation(tshId, "terrainDiffuse");
		glUniform1i(i1, 0);
		int i2 = glGetUniformLocation(tshId, "heightMap");
		glUniform1i(i2, 1);
		terrainTexture.Bind(GL_TEXTURE0);
		heightTexture.Bind(GL_TEXTURE1);
		terrainMesh.DrawElements();
		terrainTexture.Unbind();
		heightTexture.Unbind();

		// draw grass
		grassShader->bind();
		grassShader->SetMat4("view", view);
		grassShader->SetMat4("projection", projection);
		int grassId = grassShader->GetProgramId();
		int g1 = glGetUniformLocation(grassId, "heightMap");
		glUniform1i(g1, 1);
		int g2 = glGetUniformLocation(grassId, "grassDist");
		glUniform1i(g2, 2);
		int g3 = glGetUniformLocation(grassId, "grassDiffuse");
		glUniform1i(g3, 3);
		heightTexture.Bind(GL_TEXTURE1);
		grassDistTexture.Bind(GL_TEXTURE2);
		grassDiffTexture.Bind(GL_TEXTURE3);
		terrainMesh.DrawElements();
		heightTexture.Unbind();
		grassDistTexture.Unbind();
		grassDiffTexture.Unbind();

		// draw water
		waterShader->bind();
		waterShader->SetFloat("elapsedTime", time);
		waterShader->SetFloat("startTime", startTime);
		waterShader->SetMat4("view", view);
		waterShader->SetMat4("projection", projection);
		waterShader->SetVec3("lightDir", glm::vec3(-1.0, -1.0, -1.0));
		waterShader->SetVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		waterShader->SetFloat("ambientStrength", 0.25);
		waterShader->SetFloat("diffuseStrength", 0.65);
		waterShader->SetFloat("specularStrength", 0.1);
		waterShader->SetFloat("shininess", 16.0);
		waterShader->SetVec3("ripple", ripplePoint.x, ripplePoint.y, ripplePoint.z);
		waterShader->SetInteger("waterTexture", 1);
		waterTexture.Bind(GL_TEXTURE1);
		waterMesh.DrawElements();
		waterTexture.Unbind();

		//Swap front and back buffers 
		glfwSwapBuffers(window);	

		// Poll for and process events 
		glfwPollEvents();
	}

	delete waterShader;
	//delete terrainShader;

	glfwTerminate();
	return 0;
}