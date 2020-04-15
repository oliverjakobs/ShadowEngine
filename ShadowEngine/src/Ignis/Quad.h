#ifndef IGNIS_QUAD_H
#define IGNIS_QUAD_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "VertexArray.h"

typedef struct
{
	IgnisVertexArray vao;
} IgnisQuad;

void ignisCreateQuad(IgnisQuad* quad, GLfloat* vertices, size_t vertices_count, IgnisBufferElement* layout, size_t layout_size, GLuint* indices, size_t element_count);
void ignisCreateQuadTextured(IgnisQuad* quad);

void ignisDeleteQuad(IgnisQuad* quad);

void ignisBindQuad(IgnisQuad* quad);

void ignisDrawQuadElements(IgnisQuad* quad, GLenum mode);

#ifdef __cplusplus
}
#endif

#endif /* !IGNIS_QUAD_H */