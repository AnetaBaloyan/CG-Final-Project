#pragma once
#include <string>
#include <GL/glew.h>

class Texture
{
private:
	GLuint m_textureID;
	std::string m_filePath;
	unsigned char* m_localBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const std::string& path);
	~Texture();

	void Bind(int slot = 0) const;
	void Unbind()const;

	int GetWidth() const { return m_Width; }
	int GetHeight() const { return m_Height;  }
};