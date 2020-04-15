#ifndef IGNIS_BUFFER_H
#define IGNIS_BUFFER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../glad/glad.h"

	/* Buffer */
	typedef struct
	{
		GLuint name;
		GLenum target;
	} IgnisBuffer;

	/* buffer generation */
	int ignisGenerateBuffer(IgnisBuffer* buffer, GLenum target);
	void ignisGenerateArrayBuffer(IgnisBuffer* buffer, GLsizeiptr size, const void* data, GLenum usage);
	void ignisGenerateElementBuffer(IgnisBuffer* buffer, GLsizei count, const GLuint* data, GLenum usage);
	void ignisGenerateTextureBuffer(IgnisBuffer* tex_buffer, GLenum format, GLuint buffer);
	void ignisGenerateRenderBuffer(IgnisBuffer* buffer);

	void ignisDeleteBuffer(IgnisBuffer* buffer);

	/* buffer binding */
	void ignisBindBuffer(IgnisBuffer* buffer);
	void ignisUnbindBuffer(IgnisBuffer* buffer);

	/* buffer data */
	void ignisBufferData(IgnisBuffer* buffer, GLsizeiptr size, const void* data, GLenum usage);
	void ignisBufferSubData(IgnisBuffer* buffer, GLintptr offset, GLsizeiptr size, const void* data);

	void ignisElementBufferData(IgnisBuffer* buffer, GLsizei count, const GLuint* data, GLenum usage);

	void ignisBindImageTexture(IgnisBuffer* buffer, GLuint unit, GLenum access, GLenum format);
	void ignisRenderBufferStorage(IgnisBuffer* buffer, GLenum format, GLsizei width, GLsizei height);

	/* buffer mapping */
	void* ignisMapBuffer(IgnisBuffer* buffer, GLenum access);
	void* ignisMapBufferRange(IgnisBuffer* buffer, GLintptr offset, GLsizeiptr length, GLbitfield access);
	void ignisUnmapBuffer(IgnisBuffer* buffer);

	/* vertex attrib pointer */
	void ignisVertexAttribPointerR(GLuint index, GLint size, GLboolean normalized, GLsizei stride, const void* offset);
	void ignisVertexAttribPointer(GLuint index, GLint size, GLboolean normalized, GLsizei stride, GLintptr offset);
	void ignisVertexAttribIPointer(GLuint index, GLint size, GLsizei stride, GLintptr offset);
	void ignisVertexAttribDivisor(GLuint index, GLuint divisor);

#ifdef __cplusplus
}
#endif

#endif /* !IGNIS_BUFFER_H */