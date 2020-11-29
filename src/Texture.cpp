#include "Texture.h"
#include "stb_image.h"


Texture::Texture(const std::string& path)
:m_filePath(path), m_localBuffer(nullptr),
m_Width(0), m_Height(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(1);
	m_localBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4);										
	glGenTextures(1, &m_textureID); 
	glBindTexture(GL_TEXTURE_2D, m_textureID );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (m_localBuffer)
		stbi_image_free(m_localBuffer);

}

Texture::~Texture()
{
	glDeleteTextures(1, &m_textureID);
}

void Texture::Bind(int slot)const 
{
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void Texture::Unbind() const 
{
	glBindTexture(GL_TEXTURE_2D, 0);
}