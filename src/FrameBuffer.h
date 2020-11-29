#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <vector>
#include <GL/glew.h>

class FrameBuffer
{
  public:
	FrameBuffer(int width, int height);
	~FrameBuffer();
	void Bind();
	void Unbind();
	void BindTexture(int slot);

  private:
	unsigned int m_frameBufferID;
	unsigned int m_textureID;
	int InitTextureAttachment(int width, int height);
	int InitRenderBufferAttachment(int width, int height);
};
#endif