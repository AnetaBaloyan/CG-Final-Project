#include "Mesh.h"
#include <iostream>


Mesh::Mesh(GLfloat* vertices, int count) {
	glGenBuffers(1, &vboId);
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, count * sizeof(float), vertices, GL_STATIC_DRAW);
	vertexCount = count;
	stride = 0;
}

Mesh::~Mesh() {
	layout.clear();
}

void Mesh::AddLayout(int element) {
	layout.push_back(element);
	stride += element;
}

void Mesh::GenerateLayout() {
	GLintptr vertexOffset = 0 ;
	for (int i = 0; i < layout.size(); i++) {
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, layout[i], GL_FLOAT, false, stride * sizeof(float), (GLvoid*)(vertexOffset));
		vertexOffset += layout[i] * sizeof(float);
	}
}

void Mesh::Draw() {
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	GenerateLayout();
	glDrawArrays(GL_TRIANGLES, 0, vertexCount/ stride);
}

void Mesh::BindIndexBuffer(GLuint* indexBuffer, GLint indCount) {
	indexCount = indCount;
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indCount * sizeof(GLuint), indexBuffer, GL_STATIC_DRAW);
}


void Mesh::DrawElements() {
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	GenerateLayout();
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, nullptr);
}