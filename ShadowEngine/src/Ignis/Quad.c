#include "Quad.h"

#include "Ignis.h"

void ignisCreateQuad(IgnisQuad* quad, GLfloat* vertices, size_t vertices_count, IgnisBufferElement* layout, size_t layout_size, GLuint* indices, size_t element_count)
{
	ignisGenerateVertexArray(&quad->vao);

	ignisAddArrayBufferLayout(&quad->vao, sizeof(GLfloat) * vertices_count, vertices, GL_STATIC_DRAW, layout, layout_size);
	ignisLoadElementBuffer(&quad->vao, indices, element_count, GL_STATIC_DRAW);
}

void ignisCreateQuadTextured(IgnisQuad* quad)
{
	GLfloat vertices[] =
	{
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};

	IgnisBufferElement layout[] =
	{
		{GL_FLOAT, 3, GL_FALSE},
		{GL_FLOAT, 2, GL_FALSE}
	};

	GLuint indices[] = { 0, 1, 2, 2, 3, 0 };
	ignisCreateQuad(quad, vertices, 4 * 5, layout, 2, indices, 6);
}

void ignisDeleteQuad(IgnisQuad* quad)
{
	ignisDeleteVertexArray(&quad->vao);
}

void ignisBindQuad(IgnisQuad* quad)
{
	ignisBindVertexArray(&quad->vao);
}

void ignisDrawQuadElements(IgnisQuad* quad, GLenum mode)
{
	ignisBindQuad(quad);
	glDrawElements(mode, quad->vao.element_count, GL_UNSIGNED_INT, NULL);
}
