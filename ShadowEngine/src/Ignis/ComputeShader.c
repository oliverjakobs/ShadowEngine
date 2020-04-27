#include "ComputeShader.h"

#include "Ignis.h"

int ignisCreateComputeShader(IgnisComputeShader* shader, const char* path)
{
	char* src = ignisReadFile(path, NULL);
	if (!src)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Failed to read file: %s", path);
		return IGNIS_FAILURE;
	}

	int status = ignisCreateComputeShaderSrc(shader, src);

	free(src);

	return IGNIS_SUCCESS;
}

int ignisCreateComputeShaderSrc(IgnisComputeShader* shader, const char* src)
{
	if (!(shader && src))
		return IGNIS_FAILURE;

	GLenum types[] = { GL_COMPUTE_SHADER };
	const char* sources[] = { src };

	shader->program.program = ignisCreateShaderProgram(types, sources, 1);

	/* set shader info */
	glGetProgramiv(shader->program.program, GL_COMPUTE_WORK_GROUP_SIZE, shader->work_group_size);

	return IGNIS_SUCCESS;
}

void ignisDeleteComputeShader(IgnisComputeShader* shader)
{
	ignisDeleteShader(&shader->program);
}

void ignisDispatchCompute(IgnisComputeShader* shader, GLuint groups_x, GLuint groups_y, GLuint groups_z)
{
	glDispatchCompute(groups_x, groups_y, groups_z);
}
