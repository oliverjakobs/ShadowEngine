#include "VertexArray.h"

#include "Ignis.h"

int ignisGenerateVertexArray(IgnisVertexArray* vao)
{
	if (!vao) return IGNIS_FAILURE;

	glGenVertexArrays(1, &vao->name);
	glBindVertexArray(vao->name);

	vao->vertex_attrib_index = 0;

	vao->array_buffers = (IgnisBuffer*)malloc(IGNIS_BUFFER_ARRAY_INITIAL_SIZE * sizeof(IgnisBuffer));

	if (!vao->array_buffers) return IGNIS_FAILURE;

	vao->array_buffer_capacity = IGNIS_BUFFER_ARRAY_INITIAL_SIZE;
	vao->array_buffer_count = 0;

	vao->element_buffer.name = 0;
	vao->element_buffer.target = 0;
	vao->element_count = 0;

	return IGNIS_SUCCESS;
}

void ignisDeleteVertexArray(IgnisVertexArray* vao)
{
	for (size_t i = 0; i < vao->array_buffer_count; i++)
	{
		ignisDeleteBuffer(&vao->array_buffers[i]);
	}

	free(vao->array_buffers);
	vao->array_buffer_count = vao->array_buffer_capacity = 0;

	if (vao->element_buffer.target == GL_ELEMENT_ARRAY_BUFFER)
		ignisDeleteBuffer(&vao->element_buffer);

	glDeleteVertexArrays(1, &vao->name);
	glBindVertexArray(0);
}

void ignisBindVertexArray(IgnisVertexArray* vao)
{
	glBindVertexArray((vao) ? vao->name : 0);
}

int _ignisInsertArrayBuffer(IgnisVertexArray* vao, IgnisBuffer* buffer)
{
	if (vao->array_buffer_count == vao->array_buffer_capacity)
	{
		vao->array_buffer_capacity *= IGNIS_BUFFER_ARRAY_GROWTH_FACTOR;
		void* temp = realloc(vao->array_buffers, vao->array_buffer_capacity * sizeof(IgnisBuffer));

		if (!temp)
		{
			_ignisErrorCallback(IGNIS_ERROR, "[VAO] Failed to grow dynamic buffer array");
			return IGNIS_FAILURE;
		}

		vao->array_buffers = (IgnisBuffer*)temp;
	}

	memcpy(vao->array_buffers + vao->array_buffer_count++, buffer, sizeof(IgnisBuffer));
	return IGNIS_SUCCESS;
}

int ignisAddArrayBuffer(IgnisVertexArray* vao, GLsizeiptr size, const void* data, GLenum usage)
{
	ignisBindVertexArray(vao);

	IgnisBuffer buffer;
	if (ignisGenerateArrayBuffer(&buffer, size, data, usage) == IGNIS_SUCCESS)
		return _ignisInsertArrayBuffer(vao, &buffer);

	return IGNIS_FAILURE;
}

int ignisAddArrayBufferLayout(IgnisVertexArray* vao, GLsizeiptr size, const void* data, GLenum usage, IgnisBufferElement* layout, size_t count)
{
	ignisBindVertexArray(vao);

	IgnisBuffer buffer;

	if (ignisGenerateArrayBuffer(&buffer, size, data, usage) == IGNIS_FAILURE)
		return IGNIS_FAILURE;

	if (_ignisInsertArrayBuffer(vao, &buffer) == IGNIS_FAILURE)
		return IGNIS_FAILURE;

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

	return IGNIS_SUCCESS;
}

int ignisLoadElementBuffer(IgnisVertexArray* vao, GLuint* indices, GLsizei count, GLenum usage)
{
	if (ignisGenerateElementBuffer(&vao->element_buffer, count, indices, usage) == IGNIS_SUCCESS)
	{
		vao->element_count = count;
		return IGNIS_SUCCESS;
	}
	return IGNIS_FAILURE;
}