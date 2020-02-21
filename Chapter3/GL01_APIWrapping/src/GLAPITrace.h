#include <string>
#include <inttypes.h>

namespace
{
	GL4API apiHook;
} // namespace

using PFNGETGLPROC = void* (const char*);

#define E2S( en ) Enum2String( en ).c_str()
extern std::string Enum2String( GLenum e );

void GLTracer_glCullFace(GLenum mode)
{
	printf("glCullFace(" "%s)\n", E2S(mode));
	apiHook.glCullFace(mode);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glPolygonMode(GLenum face, GLenum mode)
{
	printf("glPolygonMode(" "%s, %s)\n", E2S(face), E2S(mode));
	apiHook.glPolygonMode(face, mode);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
	printf("glScissor(" "%i, %i, %i, %i)\n", x, y, width, height);
	apiHook.glScissor(x, y, width, height);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
	printf("glTexParameterf(" "%s, %s, %f)\n", E2S(target), E2S(pname), param);
	apiHook.glTexParameterf(target, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
	printf("glTexParameterfv(" "%s, %s, %p)\n", E2S(target), E2S(pname), params);
	apiHook.glTexParameterfv(target, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	printf("glTexParameteri(" "%s, %s, %i)\n", E2S(target), E2S(pname), param);
	apiHook.glTexParameteri(target, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
	printf("glTexParameteriv(" "%s, %s, %p)\n", E2S(target), E2S(pname), params);
	apiHook.glTexParameteriv(target, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)
{
	printf("glTexImage2D(" "%s, %i, %i, %i, %i, %i, %s, %s, %p)\n", E2S(target), level, internalformat, width, height, border, E2S(format), E2S(type), pixels);
	apiHook.glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClear(GLbitfield mask)
{
	printf("glClear(" "%u)\n", (unsigned int)(mask));
	apiHook.glClear(mask);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	printf("glClearColor(" "%f, %f, %f, %f)\n", red, green, blue, alpha);
	apiHook.glClearColor(red, green, blue, alpha);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClearStencil(GLint s)
{
	printf("glClearStencil(" "%i)\n", s);
	apiHook.glClearStencil(s);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClearDepth(GLdouble depth)
{
	printf("glClearDepth(" "%f)\n", depth);
	apiHook.glClearDepth(depth);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	printf("glColorMask(" "%u, %u, %u, %u)\n", (unsigned int)(red), (unsigned int)(green), (unsigned int)(blue), (unsigned int)(alpha));
	apiHook.glColorMask(red, green, blue, alpha);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDepthMask(GLboolean flag)
{
	printf("glDepthMask(" "%u)\n", (unsigned int)(flag));
	apiHook.glDepthMask(flag);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDisable(GLenum cap)
{
	printf("glDisable(" "%s)\n", E2S(cap));
	apiHook.glDisable(cap);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glEnable(GLenum cap)
{
	printf("glEnable(" "%s)\n", E2S(cap));
	apiHook.glEnable(cap);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glFinish()
{
	printf("glFinish()\n");
	apiHook.glFinish();
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glFlush()
{
	printf("glFlush()\n");
	apiHook.glFlush();
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBlendFunc(GLenum sfactor, GLenum dfactor)
{
	printf("glBlendFunc(" "%s, %s)\n", E2S(sfactor), E2S(dfactor));
	apiHook.glBlendFunc(sfactor, dfactor);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDepthFunc(GLenum func)
{
	printf("glDepthFunc(" "%s)\n", E2S(func));
	apiHook.glDepthFunc(func);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glPixelStorei(GLenum pname, GLint param)
{
	printf("glPixelStorei(" "%s, %i)\n", E2S(pname), param);
	apiHook.glPixelStorei(pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glReadBuffer(GLenum src)
{
	printf("glReadBuffer(" "%s)\n", E2S(src));
	apiHook.glReadBuffer(src);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void* pixels)
{
	printf("glReadPixels(" "%i, %i, %i, %i, %s, %s, %p)\n", x, y, width, height, E2S(format), E2S(type), pixels);
	apiHook.glReadPixels(x, y, width, height, format, type, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

GLenum GLTracer_glGetError()
{
	printf("glGetError()\n");
	GLenum const r = apiHook.glGetError();
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glGetIntegerv(GLenum pname, GLint* data)
{
	printf("glGetIntegerv(" "%s, %p)\n", E2S(pname), data);
	apiHook.glGetIntegerv(pname, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

const GLubyte* GLTracer_glGetString(GLenum name)
{
	printf("glGetString(" "%s)\n", E2S(name));
	const GLubyte* const r = apiHook.glGetString(name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void* pixels)
{
	printf("glGetTexImage(" "%s, %i, %s, %s, %p)\n", E2S(target), level, E2S(format), E2S(type), pixels);
	apiHook.glGetTexImage(target, level, format, type, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params)
{
	printf("glGetTexLevelParameteriv(" "%s, %i, %s, %p)\n", E2S(target), level, E2S(pname), params);
	apiHook.glGetTexLevelParameteriv(target, level, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	printf("glViewport(" "%i, %i, %i, %i)\n", x, y, width, height);
	apiHook.glViewport(x, y, width, height);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	printf("glDrawArrays(" "%s, %i, %i)\n", E2S(mode), first, count);
	apiHook.glDrawArrays(mode, first, count);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void* indices)
{
	printf("glDrawElements(" "%s, %i, %s, %p)\n", E2S(mode), count, E2S(type), indices);
	apiHook.glDrawElements(mode, count, type, indices);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	printf("glTexSubImage2D(" "%s, %i, %i, %i, %i, %i, %s, %s, %p)\n", E2S(target), level, xoffset, yoffset, width, height, E2S(format), E2S(type), pixels);
	apiHook.glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDeleteTextures(GLsizei n, const GLuint* textures)
{
	printf("glDeleteTextures(" "%i, %p)\n", n, textures);
	apiHook.glDeleteTextures(n, textures);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGenTextures(GLsizei n, GLuint* textures)
{
	printf("glGenTextures(" "%i, %p)\n", n, textures);
	apiHook.glGenTextures(n, textures);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels)
{
	printf("glTexImage3D(" "%s, %i, %i, %i, %i, %i, %i, %s, %s, %p)\n", E2S(target), level, internalformat, width, height, depth, border, E2S(format), E2S(type), pixels);
	apiHook.glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glActiveTexture(GLenum texture)
{
	printf("glActiveTexture(" "%s)\n", E2S(texture));
	apiHook.glActiveTexture(texture);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data)
{
	printf("glCompressedTexImage3D(" "%s, %i, %s, %i, %i, %i, %i, %i, %p)\n", E2S(target), level, E2S(internalformat), width, height, depth, border, imageSize, data);
	apiHook.glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void* data)
{
	printf("glCompressedTexImage2D(" "%s, %i, %s, %i, %i, %i, %i, %p)\n", E2S(target), level, E2S(internalformat), width, height, border, imageSize, data);
	apiHook.glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetCompressedTexImage(GLenum target, GLint level, void* img)
{
	printf("glGetCompressedTexImage(" "%s, %i, %p)\n", E2S(target), level, img);
	apiHook.glGetCompressedTexImage(target, level, img);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGenQueries(GLsizei n, GLuint* ids)
{
	printf("glGenQueries(" "%i, %p)\n", n, ids);
	apiHook.glGenQueries(n, ids);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDeleteQueries(GLsizei n, const GLuint* ids)
{
	printf("glDeleteQueries(" "%i, %p)\n", n, ids);
	apiHook.glDeleteQueries(n, ids);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBeginQuery(GLenum target, GLuint id)
{
	printf("glBeginQuery(" "%s, %u)\n", E2S(target), id);
	apiHook.glBeginQuery(target, id);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glEndQuery(GLenum target)
{
	printf("glEndQuery(" "%s)\n", E2S(target));
	apiHook.glEndQuery(target);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetQueryObjectiv(GLuint id, GLenum pname, GLint* params)
{
	printf("glGetQueryObjectiv(" "%u, %s, %p)\n", id, E2S(pname), params);
	apiHook.glGetQueryObjectiv(id, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBindBuffer(GLenum target, GLuint buffer)
{
	printf("glBindBuffer(" "%s, %u)\n", E2S(target), buffer);
	apiHook.glBindBuffer(target, buffer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDeleteBuffers(GLsizei n, const GLuint* buffers)
{
	printf("glDeleteBuffers(" "%i, %p)\n", n, buffers);
	apiHook.glDeleteBuffers(n, buffers);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGenBuffers(GLsizei n, GLuint* buffers)
{
	printf("glGenBuffers(" "%i, %p)\n", n, buffers);
	apiHook.glGenBuffers(n, buffers);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage)
{
	printf("glBufferData(" "%s, %" PRId64", %p, %s)\n", E2S(target), size, data, E2S(usage));
	apiHook.glBufferData(target, size, data, usage);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void* data)
{
	printf("glBufferSubData(" "%s, %" PRId64", %" PRId64", %p)\n", E2S(target), offset, size, data);
	apiHook.glBufferSubData(target, offset, size, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDrawBuffers(GLsizei n, const GLenum* bufs)
{
	printf("glDrawBuffers(" "%i, %p)\n", n, bufs);
	apiHook.glDrawBuffers(n, bufs);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glAttachShader(GLuint program, GLuint shader)
{
	printf("glAttachShader(" "%u, %u)\n", program, shader);
	apiHook.glAttachShader(program, shader);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBindAttribLocation(GLuint program, GLuint index, const GLchar* name)
{
	printf("glBindAttribLocation(" "%u, %u, %p)\n", program, index, name);
	apiHook.glBindAttribLocation(program, index, name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCompileShader(GLuint shader)
{
	printf("glCompileShader(" "%u)\n", shader);
	apiHook.glCompileShader(shader);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

GLuint GLTracer_glCreateProgram()
{
	printf("glCreateProgram()\n");
	GLuint const r = apiHook.glCreateProgram();
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

GLuint GLTracer_glCreateShader(GLenum type)
{
	printf("glCreateShader(" "%s)\n", E2S(type));
	GLuint const r = apiHook.glCreateShader(type);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glDeleteProgram(GLuint program)
{
	printf("glDeleteProgram(" "%u)\n", program);
	apiHook.glDeleteProgram(program);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDeleteShader(GLuint shader)
{
	printf("glDeleteShader(" "%u)\n", shader);
	apiHook.glDeleteShader(shader);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDisableVertexAttribArray(GLuint index)
{
	printf("glDisableVertexAttribArray(" "%u)\n", index);
	apiHook.glDisableVertexAttribArray(index);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glEnableVertexAttribArray(GLuint index)
{
	printf("glEnableVertexAttribArray(" "%u)\n", index);
	apiHook.glEnableVertexAttribArray(index);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	printf("glGetActiveAttrib(" "%u, %u, %i, %p, %p, %p, %p)\n", program, index, bufSize, length, size, type, name);
	apiHook.glGetActiveAttrib(program, index, bufSize, length, size, type, name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name)
{
	printf("glGetActiveUniform(" "%u, %u, %i, %p, %p, %p, %p)\n", program, index, bufSize, length, size, type, name);
	apiHook.glGetActiveUniform(program, index, bufSize, length, size, type, name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

GLint GLTracer_glGetAttribLocation(GLuint program, const GLchar* name)
{
	printf("glGetAttribLocation(" "%u, %p)\n", program, name);
	GLint const r = apiHook.glGetAttribLocation(program, name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glGetProgramiv(GLuint program, GLenum pname, GLint* params)
{
	printf("glGetProgramiv(" "%u, %s, %p)\n", program, E2S(pname), params);
	apiHook.glGetProgramiv(program, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
	printf("glGetProgramInfoLog(" "%u, %i, %p, %p)\n", program, bufSize, length, infoLog);
	apiHook.glGetProgramInfoLog(program, bufSize, length, infoLog);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetShaderiv(GLuint shader, GLenum pname, GLint* params)
{
	printf("glGetShaderiv(" "%u, %s, %p)\n", shader, E2S(pname), params);
	apiHook.glGetShaderiv(shader, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog)
{
	printf("glGetShaderInfoLog(" "%u, %i, %p, %p)\n", shader, bufSize, length, infoLog);
	apiHook.glGetShaderInfoLog(shader, bufSize, length, infoLog);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

GLint GLTracer_glGetUniformLocation(GLuint program, const GLchar* name)
{
	printf("glGetUniformLocation(" "%u, %p)\n", program, name);
	GLint const r = apiHook.glGetUniformLocation(program, name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

GLboolean GLTracer_glIsProgram(GLuint program)
{
	printf("glIsProgram(" "%u)\n", program);
	GLboolean const r = apiHook.glIsProgram(program);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

GLboolean GLTracer_glIsShader(GLuint shader)
{
	printf("glIsShader(" "%u)\n", shader);
	GLboolean const r = apiHook.glIsShader(shader);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glLinkProgram(GLuint program)
{
	printf("glLinkProgram(" "%u)\n", program);
	apiHook.glLinkProgram(program);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glShaderSource(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length)
{
	printf("glShaderSource(" "%u, %i, %p, %p)\n", shader, count, string, length);
	apiHook.glShaderSource(shader, count, string, length);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUseProgram(GLuint program)
{
	printf("glUseProgram(" "%u)\n", program);
	apiHook.glUseProgram(program);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniform1f(GLint location, GLfloat v0)
{
	printf("glUniform1f(" "%i, %f)\n", location, v0);
	apiHook.glUniform1f(location, v0);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniform1i(GLint location, GLint v0)
{
	printf("glUniform1i(" "%i, %i)\n", location, v0);
	apiHook.glUniform1i(location, v0);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniform1fv(GLint location, GLsizei count, const GLfloat* value)
{
	printf("glUniform1fv(" "%i, %i, %p)\n", location, count, value);
	apiHook.glUniform1fv(location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniform3fv(GLint location, GLsizei count, const GLfloat* value)
{
	printf("glUniform3fv(" "%i, %i, %p)\n", location, count, value);
	apiHook.glUniform3fv(location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniform4fv(GLint location, GLsizei count, const GLfloat* value)
{
	printf("glUniform4fv(" "%i, %i, %p)\n", location, count, value);
	apiHook.glUniform4fv(location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniform1iv(GLint location, GLsizei count, const GLint* value)
{
	printf("glUniform1iv(" "%i, %i, %p)\n", location, count, value);
	apiHook.glUniform1iv(location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	printf("glUniformMatrix3fv(" "%i, %i, %u, %p)\n", location, count, (unsigned int)(transpose), value);
	apiHook.glUniformMatrix3fv(location, count, transpose, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	printf("glUniformMatrix4fv(" "%i, %i, %u, %p)\n", location, count, (unsigned int)(transpose), value);
	apiHook.glUniformMatrix4fv(location, count, transpose, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glValidateProgram(GLuint program)
{
	printf("glValidateProgram(" "%u)\n", program);
	apiHook.glValidateProgram(program);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer)
{
	printf("glVertexAttribPointer(" "%u, %i, %s, %u, %i, %p)\n", index, size, E2S(type), (unsigned int)(normalized), stride, pointer);
	apiHook.glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glEnablei(GLenum target, GLuint index)
{
	printf("glEnablei(" "%s, %u)\n", E2S(target), index);
	apiHook.glEnablei(target, index);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDisablei(GLenum target, GLuint index)
{
	printf("glDisablei(" "%s, %u)\n", E2S(target), index);
	apiHook.glDisablei(target, index);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)
{
	printf("glBindBufferRange(" "%s, %u, %u, %" PRId64", %" PRId64")\n", E2S(target), index, buffer, offset, size);
	apiHook.glBindBufferRange(target, index, buffer, offset, size);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBindBufferBase(GLenum target, GLuint index, GLuint buffer)
{
	printf("glBindBufferBase(" "%s, %u, %u)\n", E2S(target), index, buffer);
	apiHook.glBindBufferBase(target, index, buffer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBindFragDataLocation(GLuint program, GLuint color, const GLchar* name)
{
	printf("glBindFragDataLocation(" "%u, %u, %p)\n", program, color, name);
	apiHook.glBindFragDataLocation(program, color, name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

const GLubyte* GLTracer_glGetStringi(GLenum name, GLuint index)
{
	printf("glGetStringi(" "%s, %u)\n", E2S(name), index);
	const GLubyte* const r = apiHook.glGetStringi(name, index);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glBindFramebuffer(GLenum target, GLuint framebuffer)
{
	printf("glBindFramebuffer(" "%s, %u)\n", E2S(target), framebuffer);
	apiHook.glBindFramebuffer(target, framebuffer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDeleteFramebuffers(GLsizei n, const GLuint* framebuffers)
{
	printf("glDeleteFramebuffers(" "%i, %p)\n", n, framebuffers);
	apiHook.glDeleteFramebuffers(n, framebuffers);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGenFramebuffers(GLsizei n, GLuint* framebuffers)
{
	printf("glGenFramebuffers(" "%i, %p)\n", n, framebuffers);
	apiHook.glGenFramebuffers(n, framebuffers);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

GLenum GLTracer_glCheckFramebufferStatus(GLenum target)
{
	printf("glCheckFramebufferStatus(" "%s)\n", E2S(target));
	GLenum const r = apiHook.glCheckFramebufferStatus(target);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
	printf("glFramebufferTexture2D(" "%s, %s, %s, %u, %i)\n", E2S(target), E2S(attachment), E2S(textarget), texture, level);
	apiHook.glFramebufferTexture2D(target, attachment, textarget, texture, level);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
	printf("glFramebufferTexture3D(" "%s, %s, %s, %u, %i, %i)\n", E2S(target), E2S(attachment), E2S(textarget), texture, level, zoffset);
	apiHook.glFramebufferTexture3D(target, attachment, textarget, texture, level, zoffset);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGenerateMipmap(GLenum target)
{
	printf("glGenerateMipmap(" "%s)\n", E2S(target));
	apiHook.glGenerateMipmap(target);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBindVertexArray(GLuint array)
{
	printf("glBindVertexArray(" "%u)\n", array);
	apiHook.glBindVertexArray(array);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDeleteVertexArrays(GLsizei n, const GLuint* arrays)
{
	printf("glDeleteVertexArrays(" "%i, %p)\n", n, arrays);
	apiHook.glDeleteVertexArrays(n, arrays);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGenVertexArrays(GLsizei n, GLuint* arrays)
{
	printf("glGenVertexArrays(" "%i, %p)\n", n, arrays);
	apiHook.glGenVertexArrays(n, arrays);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
	printf("glDrawArraysInstanced(" "%s, %i, %i, %i)\n", E2S(mode), first, count, instancecount);
	apiHook.glDrawArraysInstanced(mode, first, count, instancecount);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount)
{
	printf("glDrawElementsInstanced(" "%s, %i, %s, %p, %i)\n", E2S(mode), count, E2S(type), indices, instancecount);
	apiHook.glDrawElementsInstanced(mode, count, type, indices, instancecount);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint* params)
{
	printf("glGetActiveUniformBlockiv(" "%u, %u, %s, %p)\n", program, uniformBlockIndex, E2S(pname), params);
	apiHook.glGetActiveUniformBlockiv(program, uniformBlockIndex, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniformBlockBinding(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
	printf("glUniformBlockBinding(" "%u, %u, %u)\n", program, uniformBlockIndex, uniformBlockBinding);
	apiHook.glUniformBlockBinding(program, uniformBlockIndex, uniformBlockBinding);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetQueryObjectui64v(GLuint id, GLenum pname, GLuint64* params)
{
	printf("glGetQueryObjectui64v(" "%u, %s, %p)\n", id, E2S(pname), params);
	apiHook.glGetQueryObjectui64v(id, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

GLint GLTracer_glGetSubroutineUniformLocation(GLuint program, GLenum shadertype, const GLchar* name)
{
	printf("glGetSubroutineUniformLocation(" "%u, %s, %p)\n", program, E2S(shadertype), name);
	GLint const r = apiHook.glGetSubroutineUniformLocation(program, shadertype, name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

GLuint GLTracer_glGetSubroutineIndex(GLuint program, GLenum shadertype, const GLchar* name)
{
	printf("glGetSubroutineIndex(" "%u, %s, %p)\n", program, E2S(shadertype), name);
	GLuint const r = apiHook.glGetSubroutineIndex(program, shadertype, name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glGetActiveSubroutineUniformiv(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint* values)
{
	printf("glGetActiveSubroutineUniformiv(" "%u, %s, %u, %s, %p)\n", program, E2S(shadertype), index, E2S(pname), values);
	apiHook.glGetActiveSubroutineUniformiv(program, shadertype, index, pname, values);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetActiveSubroutineName(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei* length, GLchar* name)
{
	printf("glGetActiveSubroutineName(" "%u, %s, %u, %i, %p, %p)\n", program, E2S(shadertype), index, bufsize, length, name);
	apiHook.glGetActiveSubroutineName(program, shadertype, index, bufsize, length, name);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glUniformSubroutinesuiv(GLenum shadertype, GLsizei count, const GLuint* indices)
{
	printf("glUniformSubroutinesuiv(" "%s, %i, %p)\n", E2S(shadertype), count, indices);
	apiHook.glUniformSubroutinesuiv(shadertype, count, indices);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetProgramStageiv(GLuint program, GLenum shadertype, GLenum pname, GLint* values)
{
	printf("glGetProgramStageiv(" "%u, %s, %s, %p)\n", program, E2S(shadertype), E2S(pname), values);
	apiHook.glGetProgramStageiv(program, shadertype, pname, values);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glPatchParameteri(GLenum pname, GLint value)
{
	printf("glPatchParameteri(" "%s, %i)\n", E2S(pname), value);
	apiHook.glPatchParameteri(pname, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetProgramBinary(GLuint program, GLsizei bufSize, GLsizei* length, GLenum* binaryFormat, void* binary)
{
	printf("glGetProgramBinary(" "%u, %i, %p, %p, %p)\n", program, bufSize, length, binaryFormat, binary);
	apiHook.glGetProgramBinary(program, bufSize, length, binaryFormat, binary);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramBinary(GLuint program, GLenum binaryFormat, const void* binary, GLsizei length)
{
	printf("glProgramBinary(" "%u, %s, %p, %i)\n", program, E2S(binaryFormat), binary, length);
	apiHook.glProgramBinary(program, binaryFormat, binary, length);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramParameteri(GLuint program, GLenum pname, GLint value)
{
	printf("glProgramParameteri(" "%u, %s, %i)\n", program, E2S(pname), value);
	apiHook.glProgramParameteri(program, pname, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramUniform1i(GLuint program, GLint location, GLint v0)
{
	printf("glProgramUniform1i(" "%u, %i, %i)\n", program, location, v0);
	apiHook.glProgramUniform1i(program, location, v0);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramUniform1f(GLuint program, GLint location, GLfloat v0)
{
	printf("glProgramUniform1f(" "%u, %i, %f)\n", program, location, v0);
	apiHook.glProgramUniform1f(program, location, v0);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramUniform2iv(GLuint program, GLint location, GLsizei count, const GLint* value)
{
	printf("glProgramUniform2iv(" "%u, %i, %i, %p)\n", program, location, count, value);
	apiHook.glProgramUniform2iv(program, location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramUniform2fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
{
	printf("glProgramUniform2fv(" "%u, %i, %i, %p)\n", program, location, count, value);
	apiHook.glProgramUniform2fv(program, location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramUniform3iv(GLuint program, GLint location, GLsizei count, const GLint* value)
{
	printf("glProgramUniform3iv(" "%u, %i, %i, %p)\n", program, location, count, value);
	apiHook.glProgramUniform3iv(program, location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramUniform3fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
{
	printf("glProgramUniform3fv(" "%u, %i, %i, %p)\n", program, location, count, value);
	apiHook.glProgramUniform3fv(program, location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramUniform4iv(GLuint program, GLint location, GLsizei count, const GLint* value)
{
	printf("glProgramUniform4iv(" "%u, %i, %i, %p)\n", program, location, count, value);
	apiHook.glProgramUniform4iv(program, location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glProgramUniform4fv(GLuint program, GLint location, GLsizei count, const GLfloat* value)
{
	printf("glProgramUniform4fv(" "%u, %i, %i, %p)\n", program, location, count, value);
	apiHook.glProgramUniform4fv(program, location, count, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBindTextures(GLuint first, GLsizei count, const GLuint* textures)
{
	printf("glBindTextures(" "%u, %i, %p)\n", first, count, textures);
	apiHook.glBindTextures(first, count, textures);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCreateTransformFeedbacks(GLsizei n, GLuint* ids)
{
	printf("glCreateTransformFeedbacks(" "%i, %p)\n", n, ids);
	apiHook.glCreateTransformFeedbacks(n, ids);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTransformFeedbackBufferBase(GLuint xfb, GLuint index, GLuint buffer)
{
	printf("glTransformFeedbackBufferBase(" "%u, %u, %u)\n", xfb, index, buffer);
	apiHook.glTransformFeedbackBufferBase(xfb, index, buffer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTransformFeedbackBufferRange(GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizei size)
{
	printf("glTransformFeedbackBufferRange(" "%u, %u, %u, %" PRId64", %i)\n", xfb, index, buffer, offset, size);
	apiHook.glTransformFeedbackBufferRange(xfb, index, buffer, offset, size);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTransformFeedbackiv(GLuint xfb, GLenum pname, GLint* param)
{
	printf("glGetTransformFeedbackiv(" "%u, %s, %p)\n", xfb, E2S(pname), param);
	apiHook.glGetTransformFeedbackiv(xfb, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTransformFeedbacki_v(GLuint xfb, GLenum pname, GLuint index, GLint* param)
{
	printf("glGetTransformFeedbacki_v(" "%u, %s, %u, %p)\n", xfb, E2S(pname), index, param);
	apiHook.glGetTransformFeedbacki_v(xfb, pname, index, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTransformFeedbacki64_v(GLuint xfb, GLenum pname, GLuint index, GLint64* param)
{
	printf("glGetTransformFeedbacki64_v(" "%u, %s, %u, %p)\n", xfb, E2S(pname), index, param);
	apiHook.glGetTransformFeedbacki64_v(xfb, pname, index, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCreateBuffers(GLsizei n, GLuint* buffers)
{
	printf("glCreateBuffers(" "%i, %p)\n", n, buffers);
	apiHook.glCreateBuffers(n, buffers);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedBufferStorage(GLuint buffer, GLsizei size, const void* data, GLbitfield flags)
{
	printf("glNamedBufferStorage(" "%u, %i, %p, %u)\n", buffer, size, data, (unsigned int)(flags));
	apiHook.glNamedBufferStorage(buffer, size, data, flags);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedBufferData(GLuint buffer, GLsizei size, const void* data, GLenum usage)
{
	printf("glNamedBufferData(" "%u, %i, %p, %s)\n", buffer, size, data, E2S(usage));
	apiHook.glNamedBufferData(buffer, size, data, usage);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizei size, const void* data)
{
	printf("glNamedBufferSubData(" "%u, %" PRId64", %i, %p)\n", buffer, offset, size, data);
	apiHook.glNamedBufferSubData(buffer, offset, size, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCopyNamedBufferSubData(GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizei size)
{
	printf("glCopyNamedBufferSubData(" "%u, %u, %" PRId64", %" PRId64", %i)\n", readBuffer, writeBuffer, readOffset, writeOffset, size);
	apiHook.glCopyNamedBufferSubData(readBuffer, writeBuffer, readOffset, writeOffset, size);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClearNamedBufferData(GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void* data)
{
	printf("glClearNamedBufferData(" "%u, %s, %s, %s, %p)\n", buffer, E2S(internalformat), E2S(format), E2S(type), data);
	apiHook.glClearNamedBufferData(buffer, internalformat, format, type, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClearNamedBufferSubData(GLuint buffer, GLenum internalformat, GLintptr offset, GLsizei size, GLenum format, GLenum type, const void* data)
{
	printf("glClearNamedBufferSubData(" "%u, %s, %" PRId64", %i, %s, %s, %p)\n", buffer, E2S(internalformat), offset, size, E2S(format), E2S(type), data);
	apiHook.glClearNamedBufferSubData(buffer, internalformat, offset, size, format, type, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void* GLTracer_glMapNamedBuffer(GLuint buffer, GLenum access)
{
	printf("glMapNamedBuffer(" "%u, %s)\n", buffer, E2S(access));
	void* const r = apiHook.glMapNamedBuffer(buffer, access);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void* GLTracer_glMapNamedBufferRange(GLuint buffer, GLintptr offset, GLsizei length, GLbitfield access)
{
	printf("glMapNamedBufferRange(" "%u, %" PRId64", %i, %u)\n", buffer, offset, length, (unsigned int)(access));
	void* const r = apiHook.glMapNamedBufferRange(buffer, offset, length, access);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

GLboolean GLTracer_glUnmapNamedBuffer(GLuint buffer)
{
	printf("glUnmapNamedBuffer(" "%u)\n", buffer);
	GLboolean const r = apiHook.glUnmapNamedBuffer(buffer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glFlushMappedNamedBufferRange(GLuint buffer, GLintptr offset, GLsizei length)
{
	printf("glFlushMappedNamedBufferRange(" "%u, %" PRId64", %i)\n", buffer, offset, length);
	apiHook.glFlushMappedNamedBufferRange(buffer, offset, length);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetNamedBufferParameteriv(GLuint buffer, GLenum pname, GLint* params)
{
	printf("glGetNamedBufferParameteriv(" "%u, %s, %p)\n", buffer, E2S(pname), params);
	apiHook.glGetNamedBufferParameteriv(buffer, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetNamedBufferParameteri64v(GLuint buffer, GLenum pname, GLint64* params)
{
	printf("glGetNamedBufferParameteri64v(" "%u, %s, %p)\n", buffer, E2S(pname), params);
	apiHook.glGetNamedBufferParameteri64v(buffer, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetNamedBufferPointerv(GLuint buffer, GLenum pname, void** params)
{
	printf("glGetNamedBufferPointerv(" "%u, %s, %p)\n", buffer, E2S(pname), params);
	apiHook.glGetNamedBufferPointerv(buffer, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizei size, void* data)
{
	printf("glGetNamedBufferSubData(" "%u, %" PRId64", %i, %p)\n", buffer, offset, size, data);
	apiHook.glGetNamedBufferSubData(buffer, offset, size, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCreateFramebuffers(GLsizei n, GLuint* framebuffers)
{
	printf("glCreateFramebuffers(" "%i, %p)\n", n, framebuffers);
	apiHook.glCreateFramebuffers(n, framebuffers);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedFramebufferRenderbuffer(GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
	printf("glNamedFramebufferRenderbuffer(" "%u, %s, %s, %u)\n", framebuffer, E2S(attachment), E2S(renderbuffertarget), renderbuffer);
	apiHook.glNamedFramebufferRenderbuffer(framebuffer, attachment, renderbuffertarget, renderbuffer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedFramebufferParameteri(GLuint framebuffer, GLenum pname, GLint param)
{
	printf("glNamedFramebufferParameteri(" "%u, %s, %i)\n", framebuffer, E2S(pname), param);
	apiHook.glNamedFramebufferParameteri(framebuffer, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level)
{
	printf("glNamedFramebufferTexture(" "%u, %s, %u, %i)\n", framebuffer, E2S(attachment), texture, level);
	apiHook.glNamedFramebufferTexture(framebuffer, attachment, texture, level);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedFramebufferTextureLayer(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
	printf("glNamedFramebufferTextureLayer(" "%u, %s, %u, %i, %i)\n", framebuffer, E2S(attachment), texture, level, layer);
	apiHook.glNamedFramebufferTextureLayer(framebuffer, attachment, texture, level, layer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedFramebufferDrawBuffer(GLuint framebuffer, GLenum buf)
{
	printf("glNamedFramebufferDrawBuffer(" "%u, %s)\n", framebuffer, E2S(buf));
	apiHook.glNamedFramebufferDrawBuffer(framebuffer, buf);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedFramebufferDrawBuffers(GLuint framebuffer, GLsizei n, const GLenum* bufs)
{
	printf("glNamedFramebufferDrawBuffers(" "%u, %i, %p)\n", framebuffer, n, bufs);
	apiHook.glNamedFramebufferDrawBuffers(framebuffer, n, bufs);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedFramebufferReadBuffer(GLuint framebuffer, GLenum src)
{
	printf("glNamedFramebufferReadBuffer(" "%u, %s)\n", framebuffer, E2S(src));
	apiHook.glNamedFramebufferReadBuffer(framebuffer, src);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glInvalidateNamedFramebufferData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments)
{
	printf("glInvalidateNamedFramebufferData(" "%u, %i, %p)\n", framebuffer, numAttachments, attachments);
	apiHook.glInvalidateNamedFramebufferData(framebuffer, numAttachments, attachments);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glInvalidateNamedFramebufferSubData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{
	printf("glInvalidateNamedFramebufferSubData(" "%u, %i, %p, %i, %i, %i, %i)\n", framebuffer, numAttachments, attachments, x, y, width, height);
	apiHook.glInvalidateNamedFramebufferSubData(framebuffer, numAttachments, attachments, x, y, width, height);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClearNamedFramebufferiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint* value)
{
	printf("glClearNamedFramebufferiv(" "%u, %s, %i, %p)\n", framebuffer, E2S(buffer), drawbuffer, value);
	apiHook.glClearNamedFramebufferiv(framebuffer, buffer, drawbuffer, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClearNamedFramebufferuiv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint* value)
{
	printf("glClearNamedFramebufferuiv(" "%u, %s, %i, %p)\n", framebuffer, E2S(buffer), drawbuffer, value);
	apiHook.glClearNamedFramebufferuiv(framebuffer, buffer, drawbuffer, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClearNamedFramebufferfv(GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat* value)
{
	printf("glClearNamedFramebufferfv(" "%u, %s, %i, %p)\n", framebuffer, E2S(buffer), drawbuffer, value);
	apiHook.glClearNamedFramebufferfv(framebuffer, buffer, drawbuffer, value);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glClearNamedFramebufferfi(GLuint framebuffer, GLenum buffer, const GLfloat depth, GLint stencil)
{
	printf("glClearNamedFramebufferfi(" "%u, %s, %f, %i)\n", framebuffer, E2S(buffer), depth, stencil);
	apiHook.glClearNamedFramebufferfi(framebuffer, buffer, depth, stencil);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBlitNamedFramebuffer(GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
	printf("glBlitNamedFramebuffer(" "%u, %u, %i, %i, %i, %i, %i, %i, %i, %i, %u, %s)\n", readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, (unsigned int)(mask), E2S(filter));
	apiHook.glBlitNamedFramebuffer(readFramebuffer, drawFramebuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

GLenum GLTracer_glCheckNamedFramebufferStatus(GLuint framebuffer, GLenum target)
{
	printf("glCheckNamedFramebufferStatus(" "%u, %s)\n", framebuffer, E2S(target));
	GLenum const r = apiHook.glCheckNamedFramebufferStatus(framebuffer, target);
	assert(apiHook.glGetError() == GL_NO_ERROR);
	return r;
}

void GLTracer_glGetNamedFramebufferParameteriv(GLuint framebuffer, GLenum pname, GLint* param)
{
	printf("glGetNamedFramebufferParameteriv(" "%u, %s, %p)\n", framebuffer, E2S(pname), param);
	apiHook.glGetNamedFramebufferParameteriv(framebuffer, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetNamedFramebufferAttachmentParameteriv(GLuint framebuffer, GLenum attachment, GLenum pname, GLint* params)
{
	printf("glGetNamedFramebufferAttachmentParameteriv(" "%u, %s, %s, %p)\n", framebuffer, E2S(attachment), E2S(pname), params);
	apiHook.glGetNamedFramebufferAttachmentParameteriv(framebuffer, attachment, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCreateRenderbuffers(GLsizei n, GLuint* renderbuffers)
{
	printf("glCreateRenderbuffers(" "%i, %p)\n", n, renderbuffers);
	apiHook.glCreateRenderbuffers(n, renderbuffers);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedRenderbufferStorage(GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height)
{
	printf("glNamedRenderbufferStorage(" "%u, %s, %i, %i)\n", renderbuffer, E2S(internalformat), width, height);
	apiHook.glNamedRenderbufferStorage(renderbuffer, internalformat, width, height);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glNamedRenderbufferStorageMultisample(GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
	printf("glNamedRenderbufferStorageMultisample(" "%u, %i, %s, %i, %i)\n", renderbuffer, samples, E2S(internalformat), width, height);
	apiHook.glNamedRenderbufferStorageMultisample(renderbuffer, samples, internalformat, width, height);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetNamedRenderbufferParameteriv(GLuint renderbuffer, GLenum pname, GLint* params)
{
	printf("glGetNamedRenderbufferParameteriv(" "%u, %s, %p)\n", renderbuffer, E2S(pname), params);
	apiHook.glGetNamedRenderbufferParameteriv(renderbuffer, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCreateTextures(GLenum target, GLsizei n, GLuint* textures)
{
	printf("glCreateTextures(" "%s, %i, %p)\n", E2S(target), n, textures);
	apiHook.glCreateTextures(target, n, textures);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureBuffer(GLuint texture, GLenum internalformat, GLuint buffer)
{
	printf("glTextureBuffer(" "%u, %s, %u)\n", texture, E2S(internalformat), buffer);
	apiHook.glTextureBuffer(texture, internalformat, buffer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureBufferRange(GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizei size)
{
	printf("glTextureBufferRange(" "%u, %s, %u, %" PRId64", %i)\n", texture, E2S(internalformat), buffer, offset, size);
	apiHook.glTextureBufferRange(texture, internalformat, buffer, offset, size);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureStorage1D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width)
{
	printf("glTextureStorage1D(" "%u, %i, %s, %i)\n", texture, levels, E2S(internalformat), width);
	apiHook.glTextureStorage1D(texture, levels, internalformat, width);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
	printf("glTextureStorage2D(" "%u, %i, %s, %i, %i)\n", texture, levels, E2S(internalformat), width, height);
	apiHook.glTextureStorage2D(texture, levels, internalformat, width, height);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
	printf("glTextureStorage3D(" "%u, %i, %s, %i, %i, %i)\n", texture, levels, E2S(internalformat), width, height, depth);
	apiHook.glTextureStorage3D(texture, levels, internalformat, width, height, depth);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureStorage2DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations)
{
	printf("glTextureStorage2DMultisample(" "%u, %i, %s, %i, %i, %u)\n", texture, samples, E2S(internalformat), width, height, (unsigned int)(fixedsamplelocations));
	apiHook.glTextureStorage2DMultisample(texture, samples, internalformat, width, height, fixedsamplelocations);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureStorage3DMultisample(GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations)
{
	printf("glTextureStorage3DMultisample(" "%u, %i, %s, %i, %i, %i, %u)\n", texture, samples, E2S(internalformat), width, height, depth, (unsigned int)(fixedsamplelocations));
	apiHook.glTextureStorage3DMultisample(texture, samples, internalformat, width, height, depth, fixedsamplelocations);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void* pixels)
{
	printf("glTextureSubImage1D(" "%u, %i, %i, %i, %s, %s, %p)\n", texture, level, xoffset, width, E2S(format), E2S(type), pixels);
	apiHook.glTextureSubImage1D(texture, level, xoffset, width, format, type, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels)
{
	printf("glTextureSubImage2D(" "%u, %i, %i, %i, %i, %i, %s, %s, %p)\n", texture, level, xoffset, yoffset, width, height, E2S(format), E2S(type), pixels);
	apiHook.glTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, type, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels)
{
	printf("glTextureSubImage3D(" "%u, %i, %i, %i, %i, %i, %i, %i, %s, %s, %p)\n", texture, level, xoffset, yoffset, zoffset, width, height, depth, E2S(format), E2S(type), pixels);
	apiHook.glTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCompressedTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void* data)
{
	printf("glCompressedTextureSubImage1D(" "%u, %i, %i, %i, %s, %i, %p)\n", texture, level, xoffset, width, E2S(format), imageSize, data);
	apiHook.glCompressedTextureSubImage1D(texture, level, xoffset, width, format, imageSize, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCompressedTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void* data)
{
	printf("glCompressedTextureSubImage2D(" "%u, %i, %i, %i, %i, %i, %s, %i, %p)\n", texture, level, xoffset, yoffset, width, height, E2S(format), imageSize, data);
	apiHook.glCompressedTextureSubImage2D(texture, level, xoffset, yoffset, width, height, format, imageSize, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCompressedTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data)
{
	printf("glCompressedTextureSubImage3D(" "%u, %i, %i, %i, %i, %i, %i, %i, %s, %i, %p)\n", texture, level, xoffset, yoffset, zoffset, width, height, depth, E2S(format), imageSize, data);
	apiHook.glCompressedTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCopyTextureSubImage1D(GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
	printf("glCopyTextureSubImage1D(" "%u, %i, %i, %i, %i, %i)\n", texture, level, xoffset, x, y, width);
	apiHook.glCopyTextureSubImage1D(texture, level, xoffset, x, y, width);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCopyTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	printf("glCopyTextureSubImage2D(" "%u, %i, %i, %i, %i, %i, %i, %i)\n", texture, level, xoffset, yoffset, x, y, width, height);
	apiHook.glCopyTextureSubImage2D(texture, level, xoffset, yoffset, x, y, width, height);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCopyTextureSubImage3D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
	printf("glCopyTextureSubImage3D(" "%u, %i, %i, %i, %i, %i, %i, %i, %i)\n", texture, level, xoffset, yoffset, zoffset, x, y, width, height);
	apiHook.glCopyTextureSubImage3D(texture, level, xoffset, yoffset, zoffset, x, y, width, height);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureParameterf(GLuint texture, GLenum pname, GLfloat param)
{
	printf("glTextureParameterf(" "%u, %s, %f)\n", texture, E2S(pname), param);
	apiHook.glTextureParameterf(texture, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureParameterfv(GLuint texture, GLenum pname, const GLfloat* param)
{
	printf("glTextureParameterfv(" "%u, %s, %p)\n", texture, E2S(pname), param);
	apiHook.glTextureParameterfv(texture, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureParameteri(GLuint texture, GLenum pname, GLint param)
{
	printf("glTextureParameteri(" "%u, %s, %i)\n", texture, E2S(pname), param);
	apiHook.glTextureParameteri(texture, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureParameterIiv(GLuint texture, GLenum pname, const GLint* params)
{
	printf("glTextureParameterIiv(" "%u, %s, %p)\n", texture, E2S(pname), params);
	apiHook.glTextureParameterIiv(texture, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureParameterIuiv(GLuint texture, GLenum pname, const GLuint* params)
{
	printf("glTextureParameterIuiv(" "%u, %s, %p)\n", texture, E2S(pname), params);
	apiHook.glTextureParameterIuiv(texture, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glTextureParameteriv(GLuint texture, GLenum pname, const GLint* param)
{
	printf("glTextureParameteriv(" "%u, %s, %p)\n", texture, E2S(pname), param);
	apiHook.glTextureParameteriv(texture, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGenerateTextureMipmap(GLuint texture)
{
	printf("glGenerateTextureMipmap(" "%u)\n", texture);
	apiHook.glGenerateTextureMipmap(texture);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glBindTextureUnit(GLuint unit, GLuint texture)
{
	printf("glBindTextureUnit(" "%u, %u)\n", unit, texture);
	apiHook.glBindTextureUnit(unit, texture);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTextureImage(GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void* pixels)
{
	printf("glGetTextureImage(" "%u, %i, %s, %s, %i, %p)\n", texture, level, E2S(format), E2S(type), bufSize, pixels);
	apiHook.glGetTextureImage(texture, level, format, type, bufSize, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetCompressedTextureImage(GLuint texture, GLint level, GLsizei bufSize, void* pixels)
{
	printf("glGetCompressedTextureImage(" "%u, %i, %i, %p)\n", texture, level, bufSize, pixels);
	apiHook.glGetCompressedTextureImage(texture, level, bufSize, pixels);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTextureLevelParameterfv(GLuint texture, GLint level, GLenum pname, GLfloat* params)
{
	printf("glGetTextureLevelParameterfv(" "%u, %i, %s, %p)\n", texture, level, E2S(pname), params);
	apiHook.glGetTextureLevelParameterfv(texture, level, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTextureLevelParameteriv(GLuint texture, GLint level, GLenum pname, GLint* params)
{
	printf("glGetTextureLevelParameteriv(" "%u, %i, %s, %p)\n", texture, level, E2S(pname), params);
	apiHook.glGetTextureLevelParameteriv(texture, level, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTextureParameterfv(GLuint texture, GLenum pname, GLfloat* params)
{
	printf("glGetTextureParameterfv(" "%u, %s, %p)\n", texture, E2S(pname), params);
	apiHook.glGetTextureParameterfv(texture, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTextureParameterIiv(GLuint texture, GLenum pname, GLint* params)
{
	printf("glGetTextureParameterIiv(" "%u, %s, %p)\n", texture, E2S(pname), params);
	apiHook.glGetTextureParameterIiv(texture, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTextureParameterIuiv(GLuint texture, GLenum pname, GLuint* params)
{
	printf("glGetTextureParameterIuiv(" "%u, %s, %p)\n", texture, E2S(pname), params);
	apiHook.glGetTextureParameterIuiv(texture, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetTextureParameteriv(GLuint texture, GLenum pname, GLint* params)
{
	printf("glGetTextureParameteriv(" "%u, %s, %p)\n", texture, E2S(pname), params);
	apiHook.glGetTextureParameteriv(texture, pname, params);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCreateVertexArrays(GLsizei n, GLuint* arrays)
{
	printf("glCreateVertexArrays(" "%i, %p)\n", n, arrays);
	apiHook.glCreateVertexArrays(n, arrays);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glDisableVertexArrayAttrib(GLuint vaobj, GLuint index)
{
	printf("glDisableVertexArrayAttrib(" "%u, %u)\n", vaobj, index);
	apiHook.glDisableVertexArrayAttrib(vaobj, index);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glEnableVertexArrayAttrib(GLuint vaobj, GLuint index)
{
	printf("glEnableVertexArrayAttrib(" "%u, %u)\n", vaobj, index);
	apiHook.glEnableVertexArrayAttrib(vaobj, index);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glVertexArrayElementBuffer(GLuint vaobj, GLuint buffer)
{
	printf("glVertexArrayElementBuffer(" "%u, %u)\n", vaobj, buffer);
	apiHook.glVertexArrayElementBuffer(vaobj, buffer);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride)
{
	printf("glVertexArrayVertexBuffer(" "%u, %u, %u, %" PRId64", %i)\n", vaobj, bindingindex, buffer, offset, stride);
	apiHook.glVertexArrayVertexBuffer(vaobj, bindingindex, buffer, offset, stride);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glVertexArrayVertexBuffers(GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides)
{
	printf("glVertexArrayVertexBuffers(" "%u, %u, %i, %p, %p, %p)\n", vaobj, first, count, buffers, offsets, strides);
	apiHook.glVertexArrayVertexBuffers(vaobj, first, count, buffers, offsets, strides);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glVertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex)
{
	printf("glVertexArrayAttribBinding(" "%u, %u, %u)\n", vaobj, attribindex, bindingindex);
	apiHook.glVertexArrayAttribBinding(vaobj, attribindex, bindingindex);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset)
{
	printf("glVertexArrayAttribFormat(" "%u, %u, %i, %s, %u, %u)\n", vaobj, attribindex, size, E2S(type), (unsigned int)(normalized), relativeoffset);
	apiHook.glVertexArrayAttribFormat(vaobj, attribindex, size, type, normalized, relativeoffset);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glVertexArrayAttribIFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset)
{
	printf("glVertexArrayAttribIFormat(" "%u, %u, %i, %s, %u)\n", vaobj, attribindex, size, E2S(type), relativeoffset);
	apiHook.glVertexArrayAttribIFormat(vaobj, attribindex, size, type, relativeoffset);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glVertexArrayAttribLFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset)
{
	printf("glVertexArrayAttribLFormat(" "%u, %u, %i, %s, %u)\n", vaobj, attribindex, size, E2S(type), relativeoffset);
	apiHook.glVertexArrayAttribLFormat(vaobj, attribindex, size, type, relativeoffset);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glVertexArrayBindingDivisor(GLuint vaobj, GLuint bindingindex, GLuint divisor)
{
	printf("glVertexArrayBindingDivisor(" "%u, %u, %u)\n", vaobj, bindingindex, divisor);
	apiHook.glVertexArrayBindingDivisor(vaobj, bindingindex, divisor);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetVertexArrayiv(GLuint vaobj, GLenum pname, GLint* param)
{
	printf("glGetVertexArrayiv(" "%u, %s, %p)\n", vaobj, E2S(pname), param);
	apiHook.glGetVertexArrayiv(vaobj, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetVertexArrayIndexediv(GLuint vaobj, GLuint index, GLenum pname, GLint* param)
{
	printf("glGetVertexArrayIndexediv(" "%u, %u, %s, %p)\n", vaobj, index, E2S(pname), param);
	apiHook.glGetVertexArrayIndexediv(vaobj, index, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glGetVertexArrayIndexed64iv(GLuint vaobj, GLuint index, GLenum pname, GLint64* param)
{
	printf("glGetVertexArrayIndexed64iv(" "%u, %u, %s, %p)\n", vaobj, index, E2S(pname), param);
	apiHook.glGetVertexArrayIndexed64iv(vaobj, index, pname, param);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCreateSamplers(GLsizei n, GLuint* samplers)
{
	printf("glCreateSamplers(" "%i, %p)\n", n, samplers);
	apiHook.glCreateSamplers(n, samplers);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCreateProgramPipelines(GLsizei n, GLuint* pipelines)
{
	printf("glCreateProgramPipelines(" "%i, %p)\n", n, pipelines);
	apiHook.glCreateProgramPipelines(n, pipelines);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

void GLTracer_glCreateQueries(GLenum target, GLsizei n, GLuint* ids)
{
	printf("glCreateQueries(" "%s, %i, %p)\n", E2S(target), n, ids);
	apiHook.glCreateQueries(target, n, ids);
	assert(apiHook.glGetError() == GL_NO_ERROR);
}

#define INJECT(S) api->S = &GLTracer_##S;

void InjectAPITracer4(GL4API* api)
{
	apiHook = *api;
	INJECT(glActiveTexture);
	INJECT(glAttachShader);
	INJECT(glBeginQuery);
	INJECT(glBindAttribLocation);
	INJECT(glBindBuffer);
	INJECT(glBindBufferBase);
	INJECT(glBindBufferRange);
	INJECT(glBindFragDataLocation);
	INJECT(glBindFramebuffer);
	INJECT(glBindTextureUnit);
	INJECT(glBindTextures);
	INJECT(glBindVertexArray);
	INJECT(glBlendFunc);
	INJECT(glBlitNamedFramebuffer);
	INJECT(glBufferData);
	INJECT(glBufferSubData);
	INJECT(glCheckFramebufferStatus);
	INJECT(glCheckNamedFramebufferStatus);
	INJECT(glClear);
	INJECT(glClearColor);
	INJECT(glClearDepth);
	INJECT(glClearNamedBufferData);
	INJECT(glClearNamedBufferSubData);
	INJECT(glClearNamedFramebufferfi);
	INJECT(glClearNamedFramebufferfv);
	INJECT(glClearNamedFramebufferiv);
	INJECT(glClearNamedFramebufferuiv);
	INJECT(glClearStencil);
	INJECT(glColorMask);
	INJECT(glCompileShader);
	INJECT(glCompressedTexImage2D);
	INJECT(glCompressedTexImage3D);
	INJECT(glCompressedTextureSubImage1D);
	INJECT(glCompressedTextureSubImage2D);
	INJECT(glCompressedTextureSubImage3D);
	INJECT(glCopyNamedBufferSubData);
	INJECT(glCopyTextureSubImage1D);
	INJECT(glCopyTextureSubImage2D);
	INJECT(glCopyTextureSubImage3D);
	INJECT(glCreateBuffers);
	INJECT(glCreateFramebuffers);
	INJECT(glCreateProgram);
	INJECT(glCreateProgramPipelines);
	INJECT(glCreateQueries);
	INJECT(glCreateRenderbuffers);
	INJECT(glCreateSamplers);
	INJECT(glCreateShader);
	INJECT(glCreateTextures);
	INJECT(glCreateTransformFeedbacks);
	INJECT(glCreateVertexArrays);
	INJECT(glCullFace);
	INJECT(glDeleteBuffers);
	INJECT(glDeleteFramebuffers);
	INJECT(glDeleteProgram);
	INJECT(glDeleteQueries);
	INJECT(glDeleteShader);
	INJECT(glDeleteTextures);
	INJECT(glDeleteVertexArrays);
	INJECT(glDepthFunc);
	INJECT(glDepthMask);
	INJECT(glDisable);
	INJECT(glDisableVertexArrayAttrib);
	INJECT(glDisableVertexAttribArray);
	INJECT(glDisablei);
	INJECT(glDrawArrays);
	INJECT(glDrawArraysInstanced);
	INJECT(glDrawBuffers);
	INJECT(glDrawElements);
	INJECT(glDrawElementsInstanced);
	INJECT(glEnable);
	INJECT(glEnableVertexArrayAttrib);
	INJECT(glEnableVertexAttribArray);
	INJECT(glEnablei);
	INJECT(glEndQuery);
	INJECT(glFinish);
	INJECT(glFlush);
	INJECT(glFlushMappedNamedBufferRange);
	INJECT(glFramebufferTexture2D);
	INJECT(glFramebufferTexture3D);
	INJECT(glGenBuffers);
	INJECT(glGenFramebuffers);
	INJECT(glGenQueries);
	INJECT(glGenTextures);
	INJECT(glGenVertexArrays);
	INJECT(glGenerateMipmap);
	INJECT(glGenerateTextureMipmap);
	INJECT(glGetActiveAttrib);
	INJECT(glGetActiveSubroutineName);
	INJECT(glGetActiveSubroutineUniformiv);
	INJECT(glGetActiveUniform);
	INJECT(glGetActiveUniformBlockiv);
	INJECT(glGetAttribLocation);
	INJECT(glGetCompressedTexImage);
	INJECT(glGetCompressedTextureImage);
	INJECT(glGetIntegerv);
	INJECT(glGetNamedBufferParameteri64v);
	INJECT(glGetNamedBufferParameteriv);
	INJECT(glGetNamedBufferPointerv);
	INJECT(glGetNamedBufferSubData);
	INJECT(glGetNamedFramebufferAttachmentParameteriv);
	INJECT(glGetNamedFramebufferParameteriv);
	INJECT(glGetNamedRenderbufferParameteriv);
	INJECT(glGetProgramBinary);
	INJECT(glGetProgramInfoLog);
	INJECT(glGetProgramStageiv);
	INJECT(glGetProgramiv);
	INJECT(glGetQueryObjectiv);
	INJECT(glGetQueryObjectui64v);
	INJECT(glGetShaderInfoLog);
	INJECT(glGetShaderiv);
	INJECT(glGetSubroutineIndex);
	INJECT(glGetSubroutineUniformLocation);
	INJECT(glGetTexImage);
	INJECT(glGetTexLevelParameteriv);
	INJECT(glGetTextureImage);
	INJECT(glGetTextureLevelParameterfv);
	INJECT(glGetTextureLevelParameteriv);
	INJECT(glGetTextureParameterIiv);
	INJECT(glGetTextureParameterIuiv);
	INJECT(glGetTextureParameterfv);
	INJECT(glGetTextureParameteriv);
	INJECT(glGetTransformFeedbacki64_v);
	INJECT(glGetTransformFeedbacki_v);
	INJECT(glGetTransformFeedbackiv);
	INJECT(glGetUniformLocation);
	INJECT(glGetVertexArrayIndexed64iv);
	INJECT(glGetVertexArrayIndexediv);
	INJECT(glGetVertexArrayiv);
	INJECT(glInvalidateNamedFramebufferData);
	INJECT(glInvalidateNamedFramebufferSubData);
	INJECT(glIsProgram);
	INJECT(glIsShader);
	INJECT(glLinkProgram);
	INJECT(glMapNamedBuffer);
	INJECT(glMapNamedBufferRange);
	INJECT(glNamedBufferData);
	INJECT(glNamedBufferStorage);
	INJECT(glNamedBufferSubData);
	INJECT(glNamedFramebufferDrawBuffer);
	INJECT(glNamedFramebufferDrawBuffers);
	INJECT(glNamedFramebufferParameteri);
	INJECT(glNamedFramebufferReadBuffer);
	INJECT(glNamedFramebufferRenderbuffer);
	INJECT(glNamedFramebufferTexture);
	INJECT(glNamedFramebufferTextureLayer);
	INJECT(glNamedRenderbufferStorage);
	INJECT(glNamedRenderbufferStorageMultisample);
	INJECT(glPatchParameteri);
	INJECT(glPixelStorei);
	INJECT(glPolygonMode);
	INJECT(glProgramBinary);
	INJECT(glProgramParameteri);
	INJECT(glProgramUniform1f);
	INJECT(glProgramUniform1i);
	INJECT(glProgramUniform2fv);
	INJECT(glProgramUniform2iv);
	INJECT(glProgramUniform3fv);
	INJECT(glProgramUniform3iv);
	INJECT(glProgramUniform4fv);
	INJECT(glProgramUniform4iv);
	INJECT(glReadBuffer);
	INJECT(glReadPixels);
	INJECT(glScissor);
	INJECT(glShaderSource);
	INJECT(glTexImage2D);
	INJECT(glTexImage3D);
	INJECT(glTexParameterf);
	INJECT(glTexParameterfv);
	INJECT(glTexParameteri);
	INJECT(glTexParameteriv);
	INJECT(glTexSubImage2D);
	INJECT(glTextureBuffer);
	INJECT(glTextureBufferRange);
	INJECT(glTextureParameterIiv);
	INJECT(glTextureParameterIuiv);
	INJECT(glTextureParameterf);
	INJECT(glTextureParameterfv);
	INJECT(glTextureParameteri);
	INJECT(glTextureParameteriv);
	INJECT(glTextureStorage1D);
	INJECT(glTextureStorage2D);
	INJECT(glTextureStorage2DMultisample);
	INJECT(glTextureStorage3D);
	INJECT(glTextureStorage3DMultisample);
	INJECT(glTextureSubImage1D);
	INJECT(glTextureSubImage2D);
	INJECT(glTextureSubImage3D);
	INJECT(glTransformFeedbackBufferBase);
	INJECT(glTransformFeedbackBufferRange);
	INJECT(glUniform1f);
	INJECT(glUniform1fv);
	INJECT(glUniform1i);
	INJECT(glUniform1iv);
	INJECT(glUniform3fv);
	INJECT(glUniform4fv);
	INJECT(glUniformBlockBinding);
	INJECT(glUniformMatrix3fv);
	INJECT(glUniformMatrix4fv);
	INJECT(glUniformSubroutinesuiv);
	INJECT(glUnmapNamedBuffer);
	INJECT(glUseProgram);
	INJECT(glValidateProgram);
	INJECT(glVertexArrayAttribBinding);
	INJECT(glVertexArrayAttribFormat);
	INJECT(glVertexArrayAttribIFormat);
	INJECT(glVertexArrayAttribLFormat);
	INJECT(glVertexArrayBindingDivisor);
	INJECT(glVertexArrayElementBuffer);
	INJECT(glVertexArrayVertexBuffer);
	INJECT(glVertexArrayVertexBuffers);
	INJECT(glVertexAttribPointer);
	INJECT(glViewport);
}

#define LOAD_GL_FUNC(func) api->func = ( decltype(api->func) )GetGLProc(#func);

void GetAPI4(GL4API* api, PFNGETGLPROC GetGLProc)
{
	LOAD_GL_FUNC(glActiveTexture);
	LOAD_GL_FUNC(glAttachShader);
	LOAD_GL_FUNC(glBeginQuery);
	LOAD_GL_FUNC(glBindAttribLocation);
	LOAD_GL_FUNC(glBindBuffer);
	LOAD_GL_FUNC(glBindBufferBase);
	LOAD_GL_FUNC(glBindBufferRange);
	LOAD_GL_FUNC(glBindFragDataLocation);
	LOAD_GL_FUNC(glBindFramebuffer);
	LOAD_GL_FUNC(glBindTextureUnit);
	LOAD_GL_FUNC(glBindTextures);
	LOAD_GL_FUNC(glBindVertexArray);
	LOAD_GL_FUNC(glBlendFunc);
	LOAD_GL_FUNC(glBlitNamedFramebuffer);
	LOAD_GL_FUNC(glBufferData);
	LOAD_GL_FUNC(glBufferSubData);
	LOAD_GL_FUNC(glCheckFramebufferStatus);
	LOAD_GL_FUNC(glCheckNamedFramebufferStatus);
	LOAD_GL_FUNC(glClear);
	LOAD_GL_FUNC(glClearColor);
	LOAD_GL_FUNC(glClearDepth);
	LOAD_GL_FUNC(glClearNamedBufferData);
	LOAD_GL_FUNC(glClearNamedBufferSubData);
	LOAD_GL_FUNC(glClearNamedFramebufferfi);
	LOAD_GL_FUNC(glClearNamedFramebufferfv);
	LOAD_GL_FUNC(glClearNamedFramebufferiv);
	LOAD_GL_FUNC(glClearNamedFramebufferuiv);
	LOAD_GL_FUNC(glClearStencil);
	LOAD_GL_FUNC(glColorMask);
	LOAD_GL_FUNC(glCompileShader);
	LOAD_GL_FUNC(glCompressedTexImage2D);
	LOAD_GL_FUNC(glCompressedTexImage3D);
	LOAD_GL_FUNC(glCompressedTextureSubImage1D);
	LOAD_GL_FUNC(glCompressedTextureSubImage2D);
	LOAD_GL_FUNC(glCompressedTextureSubImage3D);
	LOAD_GL_FUNC(glCopyNamedBufferSubData);
	LOAD_GL_FUNC(glCopyTextureSubImage1D);
	LOAD_GL_FUNC(glCopyTextureSubImage2D);
	LOAD_GL_FUNC(glCopyTextureSubImage3D);
	LOAD_GL_FUNC(glCreateBuffers);
	LOAD_GL_FUNC(glCreateFramebuffers);
	LOAD_GL_FUNC(glCreateProgram);
	LOAD_GL_FUNC(glCreateProgramPipelines);
	LOAD_GL_FUNC(glCreateQueries);
	LOAD_GL_FUNC(glCreateRenderbuffers);
	LOAD_GL_FUNC(glCreateSamplers);
	LOAD_GL_FUNC(glCreateShader);
	LOAD_GL_FUNC(glCreateTextures);
	LOAD_GL_FUNC(glCreateTransformFeedbacks);
	LOAD_GL_FUNC(glCreateVertexArrays);
	LOAD_GL_FUNC(glCullFace);
	LOAD_GL_FUNC(glDeleteBuffers);
	LOAD_GL_FUNC(glDeleteFramebuffers);
	LOAD_GL_FUNC(glDeleteProgram);
	LOAD_GL_FUNC(glDeleteQueries);
	LOAD_GL_FUNC(glDeleteShader);
	LOAD_GL_FUNC(glDeleteTextures);
	LOAD_GL_FUNC(glDeleteVertexArrays);
	LOAD_GL_FUNC(glDepthFunc);
	LOAD_GL_FUNC(glDepthMask);
	LOAD_GL_FUNC(glDisable);
	LOAD_GL_FUNC(glDisableVertexArrayAttrib);
	LOAD_GL_FUNC(glDisableVertexAttribArray);
	LOAD_GL_FUNC(glDisablei);
	LOAD_GL_FUNC(glDrawArrays);
	LOAD_GL_FUNC(glDrawArraysInstanced);
	LOAD_GL_FUNC(glDrawBuffers);
	LOAD_GL_FUNC(glDrawElements);
	LOAD_GL_FUNC(glDrawElementsInstanced);
	LOAD_GL_FUNC(glEnable);
	LOAD_GL_FUNC(glEnableVertexArrayAttrib);
	LOAD_GL_FUNC(glEnableVertexAttribArray);
	LOAD_GL_FUNC(glEnablei);
	LOAD_GL_FUNC(glEndQuery);
	LOAD_GL_FUNC(glFinish);
	LOAD_GL_FUNC(glFlush);
	LOAD_GL_FUNC(glFlushMappedNamedBufferRange);
	LOAD_GL_FUNC(glFramebufferTexture2D);
	LOAD_GL_FUNC(glFramebufferTexture3D);
	LOAD_GL_FUNC(glGenBuffers);
	LOAD_GL_FUNC(glGenFramebuffers);
	LOAD_GL_FUNC(glGenQueries);
	LOAD_GL_FUNC(glGenTextures);
	LOAD_GL_FUNC(glGenVertexArrays);
	LOAD_GL_FUNC(glGenerateMipmap);
	LOAD_GL_FUNC(glGenerateTextureMipmap);
	LOAD_GL_FUNC(glGetActiveAttrib);
	LOAD_GL_FUNC(glGetActiveSubroutineName);
	LOAD_GL_FUNC(glGetActiveSubroutineUniformiv);
	LOAD_GL_FUNC(glGetActiveUniform);
	LOAD_GL_FUNC(glGetActiveUniformBlockiv);
	LOAD_GL_FUNC(glGetAttribLocation);
	LOAD_GL_FUNC(glGetCompressedTexImage);
	LOAD_GL_FUNC(glGetCompressedTextureImage);
	LOAD_GL_FUNC(glGetError);
	LOAD_GL_FUNC(glGetIntegerv);
	LOAD_GL_FUNC(glGetNamedBufferParameteri64v);
	LOAD_GL_FUNC(glGetNamedBufferParameteriv);
	LOAD_GL_FUNC(glGetNamedBufferPointerv);
	LOAD_GL_FUNC(glGetNamedBufferSubData);
	LOAD_GL_FUNC(glGetNamedFramebufferAttachmentParameteriv);
	LOAD_GL_FUNC(glGetNamedFramebufferParameteriv);
	LOAD_GL_FUNC(glGetNamedRenderbufferParameteriv);
	LOAD_GL_FUNC(glGetProgramBinary);
	LOAD_GL_FUNC(glGetProgramInfoLog);
	LOAD_GL_FUNC(glGetProgramStageiv);
	LOAD_GL_FUNC(glGetProgramiv);
	LOAD_GL_FUNC(glGetQueryObjectiv);
	LOAD_GL_FUNC(glGetQueryObjectui64v);
	LOAD_GL_FUNC(glGetShaderInfoLog);
	LOAD_GL_FUNC(glGetShaderiv);
	LOAD_GL_FUNC(glGetSubroutineIndex);
	LOAD_GL_FUNC(glGetSubroutineUniformLocation);
	LOAD_GL_FUNC(glGetTexImage);
	LOAD_GL_FUNC(glGetTexLevelParameteriv);
	LOAD_GL_FUNC(glGetTextureImage);
	LOAD_GL_FUNC(glGetTextureLevelParameterfv);
	LOAD_GL_FUNC(glGetTextureLevelParameteriv);
	LOAD_GL_FUNC(glGetTextureParameterIiv);
	LOAD_GL_FUNC(glGetTextureParameterIuiv);
	LOAD_GL_FUNC(glGetTextureParameterfv);
	LOAD_GL_FUNC(glGetTextureParameteriv);
	LOAD_GL_FUNC(glGetTransformFeedbacki64_v);
	LOAD_GL_FUNC(glGetTransformFeedbacki_v);
	LOAD_GL_FUNC(glGetTransformFeedbackiv);
	LOAD_GL_FUNC(glGetUniformLocation);
	LOAD_GL_FUNC(glGetVertexArrayIndexed64iv);
	LOAD_GL_FUNC(glGetVertexArrayIndexediv);
	LOAD_GL_FUNC(glGetVertexArrayiv);
	LOAD_GL_FUNC(glInvalidateNamedFramebufferData);
	LOAD_GL_FUNC(glInvalidateNamedFramebufferSubData);
	LOAD_GL_FUNC(glIsProgram);
	LOAD_GL_FUNC(glIsShader);
	LOAD_GL_FUNC(glLinkProgram);
	LOAD_GL_FUNC(glMapNamedBuffer);
	LOAD_GL_FUNC(glMapNamedBufferRange);
	LOAD_GL_FUNC(glNamedBufferData);
	LOAD_GL_FUNC(glNamedBufferStorage);
	LOAD_GL_FUNC(glNamedBufferSubData);
	LOAD_GL_FUNC(glNamedFramebufferDrawBuffer);
	LOAD_GL_FUNC(glNamedFramebufferDrawBuffers);
	LOAD_GL_FUNC(glNamedFramebufferParameteri);
	LOAD_GL_FUNC(glNamedFramebufferReadBuffer);
	LOAD_GL_FUNC(glNamedFramebufferRenderbuffer);
	LOAD_GL_FUNC(glNamedFramebufferTexture);
	LOAD_GL_FUNC(glNamedFramebufferTextureLayer);
	LOAD_GL_FUNC(glNamedRenderbufferStorage);
	LOAD_GL_FUNC(glNamedRenderbufferStorageMultisample);
	LOAD_GL_FUNC(glPatchParameteri);
	LOAD_GL_FUNC(glPixelStorei);
	LOAD_GL_FUNC(glPolygonMode);
	LOAD_GL_FUNC(glProgramBinary);
	LOAD_GL_FUNC(glProgramParameteri);
	LOAD_GL_FUNC(glProgramUniform1f);
	LOAD_GL_FUNC(glProgramUniform1i);
	LOAD_GL_FUNC(glProgramUniform2fv);
	LOAD_GL_FUNC(glProgramUniform2iv);
	LOAD_GL_FUNC(glProgramUniform3fv);
	LOAD_GL_FUNC(glProgramUniform3iv);
	LOAD_GL_FUNC(glProgramUniform4fv);
	LOAD_GL_FUNC(glProgramUniform4iv);
	LOAD_GL_FUNC(glReadBuffer);
	LOAD_GL_FUNC(glReadPixels);
	LOAD_GL_FUNC(glScissor);
	LOAD_GL_FUNC(glShaderSource);
	LOAD_GL_FUNC(glTexImage2D);
	LOAD_GL_FUNC(glTexImage3D);
	LOAD_GL_FUNC(glTexParameterf);
	LOAD_GL_FUNC(glTexParameterfv);
	LOAD_GL_FUNC(glTexParameteri);
	LOAD_GL_FUNC(glTexParameteriv);
	LOAD_GL_FUNC(glTexSubImage2D);
	LOAD_GL_FUNC(glTextureBuffer);
	LOAD_GL_FUNC(glTextureBufferRange);
	LOAD_GL_FUNC(glTextureParameterIiv);
	LOAD_GL_FUNC(glTextureParameterIuiv);
	LOAD_GL_FUNC(glTextureParameterf);
	LOAD_GL_FUNC(glTextureParameterfv);
	LOAD_GL_FUNC(glTextureParameteri);
	LOAD_GL_FUNC(glTextureParameteriv);
	LOAD_GL_FUNC(glTextureStorage1D);
	LOAD_GL_FUNC(glTextureStorage2D);
	LOAD_GL_FUNC(glTextureStorage2DMultisample);
	LOAD_GL_FUNC(glTextureStorage3D);
	LOAD_GL_FUNC(glTextureStorage3DMultisample);
	LOAD_GL_FUNC(glTextureSubImage1D);
	LOAD_GL_FUNC(glTextureSubImage2D);
	LOAD_GL_FUNC(glTextureSubImage3D);
	LOAD_GL_FUNC(glTransformFeedbackBufferBase);
	LOAD_GL_FUNC(glTransformFeedbackBufferRange);
	LOAD_GL_FUNC(glUniform1f);
	LOAD_GL_FUNC(glUniform1fv);
	LOAD_GL_FUNC(glUniform1i);
	LOAD_GL_FUNC(glUniform1iv);
	LOAD_GL_FUNC(glUniform3fv);
	LOAD_GL_FUNC(glUniform4fv);
	LOAD_GL_FUNC(glUniformBlockBinding);
	LOAD_GL_FUNC(glUniformMatrix3fv);
	LOAD_GL_FUNC(glUniformMatrix4fv);
	LOAD_GL_FUNC(glUniformSubroutinesuiv);
	LOAD_GL_FUNC(glUnmapNamedBuffer);
	LOAD_GL_FUNC(glUseProgram);
	LOAD_GL_FUNC(glValidateProgram);
	LOAD_GL_FUNC(glVertexArrayAttribBinding);
	LOAD_GL_FUNC(glVertexArrayAttribFormat);
	LOAD_GL_FUNC(glVertexArrayAttribIFormat);
	LOAD_GL_FUNC(glVertexArrayAttribLFormat);
	LOAD_GL_FUNC(glVertexArrayBindingDivisor);
	LOAD_GL_FUNC(glVertexArrayElementBuffer);
	LOAD_GL_FUNC(glVertexArrayVertexBuffer);
	LOAD_GL_FUNC(glVertexArrayVertexBuffers);
	LOAD_GL_FUNC(glVertexAttribPointer);
	LOAD_GL_FUNC(glViewport);
}

