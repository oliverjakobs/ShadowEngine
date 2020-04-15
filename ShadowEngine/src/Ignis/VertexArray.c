#include "VertexArray.h"

#include "Ignis.h"

void ignisGenerateVertexArray(IgnisVertexArray* vao)
{
	if (vao)
	{
		glGenVertexArrays(1, &vao->name);
		glBindVertexArray(vao->name);

		vao->vertex_attrib_index = 0;

		vao->array_buffers = (IgnisBuffer*)malloc(IGNIS_BUFFER_ARRAY_INITIAL_SIZE * sizeof(IgnisBuffer));
		vao->buffer_count = IGNIS_BUFFER_ARRAY_INITIAL_SIZE;
		vao->buffer_used = 0;

		vao->element_buffer.name = 0;
		vao->element_buffer.target = 0;
		vao->element_count = 0;
	}
}

void ignisDeleteVertexArray(IgnisVertexArray* vao)
{
	for (size_t i = 0; i < vao->buffer_used; i++)
	{
		ignisDeleteBuffer(&vao->array_buffers[i]);
	}

	free(vao->array_buffers);
	vao->buffer_used = vao->buffer_count = 0;

	if (vao->element_buffer.target == GL_ELEMENT_ARRAY_BUFFER)
		ignisDeleteBuffer(&vao->element_buffer);

	glDeleteVertexArrays(1, &vao->name);
	glBindVertexArray(0);
}

void ignisBindVertexArray(IgnisVertexArray* vao)
{
	glBindVertexArray(vao->name);
}

void ignisUnbindVertexArray(IgnisVertexArray* vao)
{
	glBindVertexArray(0);
}

void _ignisInsertArrayBuffer(IgnisVertexArray* vao, IgnisBuffer* buffer)
{
	if (vao->buffer_used == vao->buffer_count)
	{
		vao->buffer_count *= IGNIS_BUFFER_ARRAY_GROWTH_FACTOR;
		void* temp = realloc(vao->array_buffers, vao->buffer_count * sizeof(IgnisBuffer));

		if (!temp)
		{
			_ignisErrorCallback(IGNIS_ERROR, "[VAO] Failed to grow dynamic buffer array");
			return;
		}

		vao->array_buffers = (IgnisBuffer*)temp;
	}

	memcpy(vao->array_buffers + vao->buffer_used++, buffer, sizeof(IgnisBuffer));
}

void ignisAddArrayBuffer(IgnisVertexArray* vao, GLsizeiptr size, const void* data, GLenum usage)
{
	ignisBindVertexArray(vao);

	IgnisBuffer buffer;
	ignisGenerateArrayBuffer(&buffer, size, data, usage);

	_ignisInsertArrayBuffer(vao, &buffer);
}

void ignisAddArrayBufferLayout(IgnisVertexArray* vao, GLsizeiptr size, const void* data, GLenum usage, IgnisBufferElement* layout, size_t count)
{
	ignisBindVertexArray(vao);

	IgnisBuffer buffer;
	ignisGenerateArrayBuffer(&buffer, size, data, usage);

	_ignisInsertArrayBuffer(vao, &buffer);

	unsigned int stride = 0;
	for (size_t i = 0; i < count; i++)
	{
		stride += ignisGetOpenGLTypeSize(layout[i].type) * layout[i].count;
	}

	unsigned offset = 0;
	for (size_t i = 0; i < count; i++)
	{
		glEnableVertexAttribArray(vao->vertex_attrib_index);
		glVertexAttribPointer(vao->vertex_attrib_index, layout[i].count, layout[i].type, layout[i].normalized ? GL_TRUE : GL_FALSE, stride, (const void*)(intptr_t)offset);

		offset += ignisGetOpenGLTypeSize(layout[i].type) * layout[i].count;
		vao->vertex_attrib_index++;
	}
}

void ignisLoadElementBuffer(IgnisVertexArray* vao, GLuint* indices, GLsizei count, GLenum usage)
{
	 ignisGenerateElementBuffer(&vao->element_buffer, count, indices, usage);

	 vao->element_count = count;
}