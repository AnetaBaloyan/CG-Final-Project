#ifndef SHADER_H
#define SHADER_H

#include <vector>
#include <GL/glew.h>
#include <string>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

class Shader
{
public:
	Shader(string vertexShader, string GeometryShader, string fragmentShader);
	~Shader();
	void bind();
	void unbind();
	GLuint GetProgramId();
	void SetVec4(const std::string &name, float x, float y, float z, float w);
	void SetVec4(const std::string &name, const glm::vec4 &value);
	void SetVec3(const std::string &name, float x, float y, float z);
	void SetVec3(const std::string &name, const glm::vec3 &value);
	void SetMat3(const std::string &name, const glm::mat3 &mat);
	void SetMat4(const std::string &name, const glm::mat4 &mat);
	void SetFloat(const std::string &name, float value);
	void SetInteger(const std::string &name, int value);

private:
	GLuint programId;

	string ParseShader(string filepath);
	GLuint CompileShader(unsigned int type, const string& source);
	GLuint CreateShader(const string& vertexShader, const string& geometryShader,const string& fragmentShader);
};

#endif
