#ifndef MESH_H
#define MESH_H

#include <vector>
#include <GL/glew.h>

class Mesh
{
	public:
		Mesh(GLfloat vertices[], int count);
		~Mesh();
		void AddLayout(int element);
		void Draw();
		void DrawElements();
		void BindIndexBuffer(GLuint* indexBuffer, GLint indCount);

	private:
		int stride;
		GLuint vboId;
		GLuint vaoId;
		GLuint index_buffer;
		GLfloat* vertices;
		std::vector<int> layout;
		int vertexCount;
		int indexCount;
		void GenerateLayout();
};

#endif