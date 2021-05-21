#pragma once

#include <glad/gl.h>

class GLTexture
{
public:
	GLTexture(GLenum type, const char* fileName);
	GLTexture(GLenum type, int width, int height, GLenum internalFormat);
	GLTexture(int w, int h, const void* img);
	~GLTexture();
	GLTexture(const GLTexture&) = delete;
	GLTexture(GLTexture&&);
	GLenum getType() const { return type_; }
	GLuint getHandle() const { return handle_; }
	GLuint64 getHandleBindless() const { return handleBindless_; }

private:
	GLenum type_ = 0;
	GLuint handle_ = 0;
	GLuint64 handleBindless_ = 0;
};
