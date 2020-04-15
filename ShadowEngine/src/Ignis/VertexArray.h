#ifndef IGNIS_VERTEX_ARRAY_H
#define IGNIS_VERTEX_ARRAY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Core/Buffer.h"

typedef struct
{
    GLenum type;
    GLsizei count;
    GLboolean normalized;
} IgnisBufferElement;

typedef struct
{
    GLuint name;
    GLuint vertex_attrib_index;

    /* dynamic buffer array */
    IgnisBuffer* array_buffers;
    size_t buffer_used;
    size_t buffer_count;

    IgnisBuffer element_buffer;
    GLsizei element_count;
} IgnisVertexArray;

void ignisGenerateVertexArray(IgnisVertexArray* vao);
void ignisDeleteVertexArray(IgnisVertexArray* vao);

void ignisBindVertexArray(IgnisVertexArray* vao);
void ignisUnbindVertexArray(IgnisVertexArray* vao);

void ignisAddArrayBuffer(IgnisVertexArray* vao, GLsizeiptr size, const void* data, GLenum usage);
void ignisAddArrayBufferLayout(IgnisVertexArray* vao, GLsizeiptr size, const void* data, GLenum usage, IgnisBufferElement* layout, size_t count);
void ignisLoadElementBuffer(IgnisVertexArray* vao, GLuint* indices, GLsizei count, GLenum usage);

#ifdef __cplusplus
}
#endif

#endif /* !IGNIS_VERTEX_ARRAY_H */