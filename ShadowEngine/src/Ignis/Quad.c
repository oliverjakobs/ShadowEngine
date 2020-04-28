#include "Quad.h"

#include "Ignis.h"

int ignisCreateQuad(IgnisQuad* quad, GLfloat* vertices, size_t vertex_count, GLenum usage, IgnisBufferElement* layout, size_t layout_size, GLuint* indices, size_t element_count)
{
	if (ignisGenerateVertexArray(&quad->vao) == IGNIS_SUCCESS)
	{
		if (ignisAddArrayBufferLayout(&quad->vao, sizeof(GLfloat) * vertex_count, vertices, usage, 0, layout, layout_size) == IGNIS_FAILURE)
			return IGNIS_FAILURE;

		quad->vertex_count = vertex_count;

		return ignisLoadElementBuffer(&quad->vao, indices, element_count, GL_STATIC_DRAW);
	}

	return IGNIS_FAILURE;
}

int ignisCreateQuadTextured(IgnisQuad* quad, GLenum usage)
{
	GLfloat vertices[] =
	{
		0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f
	};

	IgnisBufferElement layout[] =
	{
		{GL_FLOAT, 2, GL_FALSE},
		{GL_FLOAT, 2, GL_FALSE}
	};

	GLuint indices[] = { 0, 1, 2, 2, 3, 0 };
	return ignisCreateQuad(quad, vertices, 4 * 4, usage, layout, 2, indices, 6);
}

void ignisDeleteQuad(IgnisQuad* quad)
{
	ignisDeleteVertexArray(&quad->vao);
}

void ignisBindQuad(IgnisQuad* quad)
{
	ignisBindVertexArray((quad) ? &quad->vao : NULL);
}

void ignisDrawQuadElements(IgnisQuad* quad, GLenum mode)
{
	ignisBindQuad(quad);
	glDrawElements(mode, quad->vao.element_count, GL_UNSIGNED_INT, NULL);
}
