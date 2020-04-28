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
	case GL_ARRAY_BUFFER:
	case GL_ELEMENT_ARRAY_BUFFER:
		glGenBuffers(1, &name);
		break;
	default:
		_ignisErrorCallback(IGNIS_ERROR, "[Buffer] Unsupported buffer target (%d)", buffer->target);
		return IGNIS_FAILURE;
	}

	if (buffer)
	{
		buffer->name = name;
		buffer->target = target;
	}

	return name;
}

int ignisGenerateArrayBuffer(IgnisBuffer* buffer, GLsizeiptr size, const void* data, GLenum usage)
{
	if (buffer && ignisGenerateBuffer(buffer, GL_ARRAY_BUFFER))
	{
		ignisBufferData(buffer, size, data, usage);
		return IGNIS_SUCCESS;
	}

	return IGNIS_FAILURE;
}

int ignisGenerateElementBuffer(IgnisBuffer* buffer, GLsizei count, const GLuint* data, GLenum usage)
{
	if (buffer && ignisGenerateBuffer(buffer, GL_ELEMENT_ARRAY_BUFFER))
	{
		ignisElementBufferData(buffer, count, data, usage);
		return IGNIS_SUCCESS;
	}

	return IGNIS_FAILURE;
}

int ignisGenerateTextureBuffer(IgnisBuffer* tex_buffer, GLenum format, GLuint buffer)
{
	if (tex_buffer && ignisGenerateBuffer(tex_buffer, GL_TEXTURE_BUFFER))
	{
		glBindTexture(tex_buffer->target, tex_buffer->name);
		glTexBuffer(tex_buffer->target, format, buffer);
		return IGNIS_SUCCESS;
	}

	return IGNIS_FAILURE;
}

int ignisGenerateRenderBuffer(IgnisBuffer* buffer)
{
	if (buffer && ignisGenerateBuffer(buffer, GL_RENDERBUFFER))
		return IGNIS_SUCCESS;

	return IGNIS_FAILURE;
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
	case GL_ARRAY_BUFFER:
	case GL_ELEMENT_ARRAY_BUFFER:
		glBindBuffer(buffer->target, 0);
		glDeleteBuffers(1, &buffer->name);
		break;
	default:
		_ignisErrorCallback(IGNIS_ERROR, "[Buffer] Unsupported buffer target (%d)", buffer->target);
	}

	buffer->name = 0;
	buffer->target = 0;
}

void ignisBindBuffer(IgnisBuffer* buffer, GLenum target)
{
	if (buffer && buffer->target != target)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Buffer] Buffer target missmatch (%d)", target);
		return;
	}

	GLuint name = buffer ? buffer->name : 0;

	switch (target)
	{
	case GL_RENDERBUFFER: 
		glBindRenderbuffer(target, name);
		return;
	case GL_TEXTURE_BUFFER:
		glBindTexture(target, name);
	case GL_ARRAY_BUFFER:
	case GL_ELEMENT_ARRAY_BUFFER:
		glBindBuffer(target, name);
		return;
	}

	_ignisErrorCallback(IGNIS_ERROR, "[Buffer] Unsupported buffer target (%d)", target);
}

void ignisBufferData(IgnisBuffer* buffer, GLsizeiptr size, const void* data, GLenum usage)
{
	ignisBindBuffer(buffer, buffer->target);
	glBufferData(buffer->target, size, data, usage);
}

void ignisBufferSubData(IgnisBuffer* buffer, GLintptr offset, GLsizeiptr size, const void* data)
{
	ignisBindBuffer(buffer, buffer->target);
	glBufferSubData(buffer->target, offset, size, data);
}

void ignisElementBufferData(IgnisBuffer* buffer, GLsizei count, const GLuint* data, GLenum usage)
{
	if (buffer->target != GL_ELEMENT_ARRAY_BUFFER)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Buffer] Buffer target is not GL_ELEMENT_ARRAY_BUFFER");
		return;
	}

	ignisBindBuffer(buffer, buffer->target);
	glBufferData(buffer->target, count * sizeof(GLuint), data, usage);
}

void ignisBindImageTexture(IgnisBuffer* buffer, GLuint unit, GLenum access, GLenum format)
{
	if (buffer->target != GL_TEXTURE_BUFFER)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Buffer] Buffer target is not GL_TEXTURE_BUFFER");
		return;
	}

	glBindImageTexture(unit, buffer->name, 0, GL_FALSE, 0, access, format);
}

void ignisRenderBufferStorage(IgnisBuffer* buffer, GLenum format, GLsizei width, GLsizei height)
{
	if (buffer->target != GL_RENDERBUFFER)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[Buffer] Buffer target is not GL_RENDERBUFFER");
		return;
	}

	ignisBindBuffer(buffer, buffer->target);
	glRenderbufferStorage(buffer->target, format, width, height);
}

void* ignisMapBuffer(IgnisBuffer* buffer, GLenum access)
{
	ignisBindBuffer(buffer, buffer->target);
	return glMapBuffer(buffer->target, access);
}

void* ignisMapBufferRange(IgnisBuffer* buffer, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	ignisBindBuffer(buffer, buffer->target);
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