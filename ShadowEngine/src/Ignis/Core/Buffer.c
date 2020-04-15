#include "Buffer.h"

#include "../Ignis.h"

int ignisGenerateBuffer(IgnisBuffer* buffer, GLenum target)
{
	GLuint name = 0;

	switch (target)
	{
	case GL_TEXTURE_BUFFER:
		glGenTextures(1, &name);
		break;
	case GL_RENDERBUFFER:
		glGenRenderbuffers(1, &name);
		break;
	default:
		glGenBuffers(1, &name);
	}

	if (buffer)
	{
		buffer->name = name;
		buffer->target = target;
	}

	return name;
}

void ignisGenerateArrayBuffer(IgnisBuffer* buffer, GLsizeiptr size, const void* data, GLenum usage)
{
	if (!buffer) return;

	if (ignisGenerateBuffer(buffer, GL_ARRAY_BUFFER))
		ignisBufferData(buffer, size, data, usage);
}

void ignisGenerateElementBuffer(IgnisBuffer* buffer, GLsizei count, const GLuint* data, GLenum usage)
{
	if (!buffer) return;

	if (ignisGenerateBuffer(buffer, GL_ELEMENT_ARRAY_BUFFER))
		ignisElementBufferData(buffer, count, data, usage);
}

void ignisGenerateTextureBuffer(IgnisBuffer* tex_buffer, GLenum format, GLuint buffer)
{
	if (!tex_buffer) return;

	if (ignisGenerateBuffer(tex_buffer, GL_TEXTURE_BUFFER))
	{
		glBindTexture(tex_buffer->target, tex_buffer->name);
		glTexBuffer(tex_buffer->target, format, buffer);
	}
}

void ignisGenerateRenderBuffer(IgnisBuffer* buffer)
{
	if (!buffer) return;

	ignisGenerateBuffer(buffer, GL_RENDERBUFFER);
}

void ignisDeleteBuffer(IgnisBuffer* buffer)
{
	switch (buffer->target)
	{
	case GL_TEXTURE_BUFFER:
		glDeleteTextures(1, &buffer->name);
		break;
	case GL_RENDERBUFFER:
		glBindRenderbuffer(buffer->target, 0);
		glDeleteRenderbuffers(1, &buffer->name);
		break;
	default:
		glBindBuffer(buffer->target, 0);
		glDeleteBuffers(1, &buffer->name);
	}

	buffer->name = 0;
	buffer->target = 0;
}

void ignisBindBuffer(IgnisBuffer* buffer)
{
	switch (buffer->target)
	{
	case GL_RENDERBUFFER:
		glBindRenderbuffer(buffer->target, buffer->name);
		break;
	default:
		glBindBuffer(buffer->target, buffer->name);
	}
}

void ignisUnbindBuffer(IgnisBuffer* buffer)
{
	switch (buffer->target)
	{
	case GL_RENDERBUFFER:
		glBindRenderbuffer(buffer->target, 0);
		break;
	default:
		glBindBuffer(buffer->target, 0);
	}
}

void ignisBufferData(IgnisBuffer* buffer, GLsizeiptr size, const void* data, GLenum usage)
{
	ignisBindBuffer(buffer);
	glBufferData(buffer->target, size, data, usage);
}

void ignisBufferSubData(IgnisBuffer* buffer, GLintptr offset, GLsizeiptr size, const void* data)
{
	ignisBindBuffer(buffer);
	glBufferSubData(buffer->target, offset, size, data);
}

void ignisElementBufferData(IgnisBuffer* buffer, GLsizei count, const GLuint* data, GLenum usage)
{
	if (buffer->target != GL_ELEMENT_ARRAY_BUFFER)
	{
		_ignisErrorCallback(IGNIS_WARN, "[Buffer] Buffer target is not GL_ELEMENT_ARRAY_BUFFER");
		return;
	}

	ignisBindBuffer(buffer);
	glBufferData(buffer->target, count * sizeof(GLuint), data, usage);
}

void ignisBindImageTexture(IgnisBuffer* buffer, GLuint unit, GLenum access, GLenum format)
{
	if (buffer->target != GL_TEXTURE_BUFFER)
	{
		_ignisErrorCallback(IGNIS_WARN, "[Buffer] Buffer target is not GL_TEXTURE_BUFFER");
		return;
	}

	glBindImageTexture(unit, buffer->name, 0, GL_FALSE, 0, access, format);
}

void ignisRenderBufferStorage(IgnisBuffer* buffer, GLenum format, GLsizei width, GLsizei height)
{
	if (buffer->target != GL_RENDERBUFFER)
	{
		_ignisErrorCallback(IGNIS_WARN, "[Buffer] Buffer target is not GL_RENDERBUFFER");
		return;
	}

	ignisBindBuffer(buffer);
	glRenderbufferStorage(buffer->target, format, width, height);
}

void* ignisMapBuffer(IgnisBuffer* buffer, GLenum access)
{
	ignisBindBuffer(buffer);
	return glMapBuffer(buffer->target, access);
}

void* ignisMapBufferRange(IgnisBuffer* buffer, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	ignisBindBuffer(buffer);
	return glMapBufferRange(buffer->target, offset, length, access);
}

void ignisUnmapBuffer(IgnisBuffer* buffer)
{
	glUnmapBuffer(buffer->target);
}

void ignisVertexAttribPointerR(GLuint index, GLint size, GLboolean normalized, GLsizei stride, const void* offset)
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, normalized ? GL_TRUE : GL_FALSE, stride, offset);
}

void ignisVertexAttribPointer(GLuint index, GLint size, GLboolean normalized, GLsizei stride, GLintptr offset)
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, normalized ? GL_TRUE : GL_FALSE, stride * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat)));
}

void ignisVertexAttribIPointer(GLuint index, GLint size, GLsizei stride, GLintptr offset)
{
	glEnableVertexAttribArray(index);
	glVertexAttribIPointer(index, size, GL_UNSIGNED_INT, stride * sizeof(GLuint), (void*)(offset * sizeof(GLuint)));
}

void ignisVertexAttribDivisor(GLuint index, GLuint divisor)
{
	glVertexAttribDivisor(index, divisor);
}