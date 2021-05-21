#pragma once

#include <glad/gl.h>

class GLShader
{
public:
	explicit GLShader(const char* fileName);
	GLShader(GLenum type, const char* text, const char* debugFileName = "");
	~GLShader();
	GLenum getType() const { return type_; }
	GLuint getHandle() const { return handle_; }

private:
	GLenum type_;
	GLuint handle_;
};

class GLProgram
{
public:
	GLProgram(const GLShader& a);
	GLProgram(const GLShader& a, const GLShader& b);
	GLProgram(const GLShader& a, const GLShader& b, const GLShader& c);
	GLProgram(const GLShader& a, const GLShader& b, const GLShader& c, const GLShader& d, const GLShader& e);
	~GLProgram();

	void useProgram() const;
	GLuint getHandle() const { return handle_; }

private:
	GLuint handle_;
};

GLenum GLShaderTypeFromFileName(const char* fileName);

class GLBuffer
{
public:
	GLBuffer(GLsizeiptr size, const void* data, GLbitfield flags);
	~GLBuffer();

	GLuint getHandle() const { return handle_; }

private:
	GLuint handle_;
};
