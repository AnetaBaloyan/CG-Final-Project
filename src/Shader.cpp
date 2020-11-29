#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
 

Shader::Shader(string vertexShader, string geometryShader, string fragmentShader) {
	string vertexSource = ParseShader(vertexShader);
	string geometrySource = "";
	if(geometryShader != ""){
		geometrySource = ParseShader(geometryShader);
	}
	string fragmentSource = ParseShader(fragmentShader);
	programId = CreateShader(vertexSource,geometrySource, fragmentSource);
}

Shader::~Shader() {
	glDeleteProgram(programId);
}

void Shader::bind() {
	glUseProgram(programId);
}

void Shader::unbind() {
	glUseProgram(0);
}

string Shader::ParseShader(string filepath) {
	ifstream stream(filepath);
	string line;
	stringstream stringStream;

	while (getline(stream, line))
	{
		stringStream << line << '\n';
	}

	return stringStream.str();
}

GLuint  Shader::CompileShader(unsigned int type, const string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); 
	glShaderSource(id, 1, &src, nullptr); 
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << type << endl;
		cout << message << endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

GLuint Shader::CreateShader(const string& vertexShader, const string& geometryShader, const string& fragmentShader)
{
	GLuint program = glCreateProgram();
	GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	glAttachShader(program, vs);
	if (geometryShader != "") {
		GLuint gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
		glAttachShader(program, gs);
	}
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);	
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

GLuint Shader::GetProgramId() {
	return programId;
}

void Shader::SetVec4(const std::string &name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(programId, name.c_str()), x, y, z, w);
}

void Shader::SetVec4(const std::string &name, const glm::vec4 &value) 
{
	glUniform4fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
}

void Shader::SetVec3(const std::string &name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(programId, name.c_str()), x, y, z);
}

void Shader::SetVec3(const std::string &name, const glm::vec3 &value)
{
	glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, &value[0]);
}

void Shader::SetFloat(const std::string &name, float value) 
{
	glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::SetInteger(const std::string &name, int value)
{
	int i = glGetUniformLocation(programId, name.c_str());
	glUniform1i(i, value);
}
void Shader::SetMat3(const std::string &name, const glm::mat3 &mat) 
{
	glUniformMatrix3fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void Shader::SetMat4(const std::string &name, const glm::mat4 &mat) 
{
	int i = glGetUniformLocation(programId, name.c_str());
	glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}