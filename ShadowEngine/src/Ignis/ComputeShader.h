#ifndef IGNIS_COMPUTE_SHADER_H
#define IGNIS_COMPUTE_SHADER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Core/Shader.h"

typedef struct
{
	IgnisShader program;

	GLuint work_group_size[3];
} IgnisComputeShader;

int ignisCreateComputeShader(IgnisComputeShader* shader, const char* path);
int ignisCreateComputeShaderSrc(IgnisComputeShader* shader, const char* src);
void ignisDeleteComputeShader(IgnisComputeShader* shader);

void ignisDispatchCompute(IgnisComputeShader* shader, GLuint groups_x, GLuint groups_y, GLuint groups_z);

#ifdef __cplusplus
}
#endif

#endif /* !IGNIS_COMPUTE_SHADER_H */