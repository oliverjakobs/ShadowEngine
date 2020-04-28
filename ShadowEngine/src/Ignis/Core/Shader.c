#include "Shader.h"

#include "../Ignis.h"

int ignisCreateShadervf(IgnisShader* shader, const char* vert, const char* frag)
{
	char* vert_src = ignisReadFile(vert, NULL);
	if (!vert_src)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Failed to read file: %s", vert);
		return IGNIS_FAILURE;
	}

	char* frag_src = ignisReadFile(frag, NULL);
	if (!frag_src)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Failed to read file: %s", frag);
		return IGNIS_FAILURE;
	}

	int status = ignisCreateShaderSrcvf(shader, vert_src, frag_src);

	free(vert_src);
	free(frag_src);

	return IGNIS_SUCCESS;
}

int ignisCreateShadervgf(IgnisShader* shader, const char* vert, const char* geom, const char* frag)
{
	char* vert_src = ignisReadFile(vert, NULL);
	if (!vert_src)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Failed to read file: %s", vert);
		return IGNIS_FAILURE;
	}

	char* geom_src = ignisReadFile(geom, NULL);
	if (!geom_src)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Failed to read file: %s", geom);
		return IGNIS_FAILURE;
	}

	char* frag_src = ignisReadFile(frag, NULL);
	if (!frag_src)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Failed to read file: %s", frag);
		return IGNIS_FAILURE;
	}

	int status = ignisCreateShaderSrcvgf(shader, vert_src, geom_src, frag_src);

	free(vert_src);
	free(geom_src);
	free(frag_src);

	return IGNIS_SUCCESS;
}

int ignisCreateShaderSrcvf(IgnisShader* shader, const char* vert, const char* frag)
{
	if (!(shader && vert && frag))
		return IGNIS_FAILURE;

	GLenum types[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	const char* sources[] = { vert, frag };

	shader->program = ignisCreateShaderProgram(types, sources, 2);

	return IGNIS_SUCCESS;
}

int ignisCreateShaderSrcvgf(IgnisShader* shader, const char* vert, const char* geom, const char* frag)
{
	if (!(shader && vert && geom && frag))
		return IGNIS_FAILURE;

	GLenum types[] = { GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER };
	const char* sources[] = { vert, geom, frag };

	shader->program = ignisCreateShaderProgram(types, sources, 3);

	return IGNIS_SUCCESS;
}

void ignisDeleteShader(IgnisShader* shader)
{
	glDeleteProgram(shader->program);
}

void ignisUseShader(IgnisShader* shader)
{
	if (!shader)
	{
		glUseProgram(0);
	}
	else
	{
		/* check if program handle refer to an object generated by OpenGL */
		IGNIS_ASSERT(glIsProgram(shader->program));

		glUseProgram(shader->program);
	}
}

GLint ignisGetUniformLocation(const IgnisShader* shader, const char* name)
{
	GLint location = glGetUniformLocation(shader->program, name);

	if (location < 0) 
		_ignisErrorCallback(IGNIS_WARN, "[SHADER] Uniform %s not found", name);

	return location;
}

void ignisSetUniform1i(const IgnisShader* shader, const char* name, int value)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniform1il(shader, location, value);
}

void ignisSetUniform1f(const IgnisShader* shader, const char* name, float value)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniform1fl(shader, location, value);
}

void ignisSetUniform2f(const IgnisShader* shader, const char* name, const float* values)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniform2fl(shader, location, values);
}

void ignisSetUniform3f(const IgnisShader* shader, const char* name, const float* values)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniform3fl(shader, location, values);
}

void ignisSetUniform4f(const IgnisShader* shader, const char* name, const float* values)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniform4fl(shader, location, values);
}

void ignisSetUniformMat2(const IgnisShader* shader, const char* name, const float* values)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniformMat2l(shader, location, values);
}

void ignisSetUniformMat3(const IgnisShader* shader, const char* name, const float* values)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniformMat3l(shader, location, values);
}

void ignisSetUniformMat4(const IgnisShader* shader, const char* name, const float* values)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniformMat4l(shader, location, values);
}

void ignisSetUniform1iv(const IgnisShader* shader, const char* name, GLsizei count, const int* values)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniform1ivl(shader, location, count, values);
}

void ignisSetUniform1fv(const IgnisShader* shader, const char* name, GLsizei count, const float* values)
{
	GLint location = ignisGetUniformLocation(shader, name);

	if (location >= 0)
		ignisSetUniform1fvl(shader, location, count, values);
}

void ignisSetUniform1il(const IgnisShader* shader, GLint location, int value)
{
	glProgramUniform1i(shader->program, location, value);
}

void ignisSetUniform1fl(const IgnisShader* shader, GLint location, float value)
{
	glProgramUniform1f(shader->program, location, value);
}

void ignisSetUniform2fl(const IgnisShader* shader, GLint location, const float* values)
{
	glProgramUniform2fv(shader->program, location, 1, values);
}

void ignisSetUniform3fl(const IgnisShader* shader, GLint location, const float* values)
{
	glProgramUniform3fv(shader->program, location, 1, values);
}

void ignisSetUniform4fl(const IgnisShader* shader, GLint location, const float* values)
{
	glProgramUniform4fv(shader->program, location, 1, values);
}

void ignisSetUniformMat2l(const IgnisShader* shader, GLint location, const float* values)
{
	glProgramUniformMatrix2fv(shader->program, location, 1, GL_FALSE, values);
}

void ignisSetUniformMat3l(const IgnisShader* shader, GLint location, const float* values)
{
	glProgramUniformMatrix3fv(shader->program, location, 1, GL_FALSE, values);
}

void ignisSetUniformMat4l(const IgnisShader* shader, GLint location, const float* values)
{
	glProgramUniformMatrix4fv(shader->program, location, 1, GL_FALSE, values);
}

void ignisSetUniform1ivl(const IgnisShader* shader, GLint location, GLsizei count, const int* values)
{
	glProgramUniform1iv(shader->program, location, count, values);
}

void ignisSetUniform1fvl(const IgnisShader* shader, GLint location, GLsizei count, const float* values)
{
	glProgramUniform1fv(shader->program, location, count, values);
}

GLuint ignisCreateShaderProgram(GLenum* types, const char** sources, size_t count)
{
	GLuint program = glCreateProgram();

	GLuint attached_shader[6];
	GLuint shader_count = 0;

	for (size_t i = 0; i < count; i++)
	{
		GLuint shader = ignisCompileShader(types[i], sources[i]);

		if (shader == 0)
		{
			glDeleteProgram(program);
			return IGNIS_FAILURE;
		}

		glAttachShader(program, shader);
		attached_shader[shader_count] = shader;
		shader_count++;
	}

	glLinkProgram(program);

	for (size_t i = 0; i < shader_count; i++)
	{
		glDeleteShader(attached_shader[i]);
		glDetachShader(program, attached_shader[i]);
	}

	GLint result = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result == GL_FALSE)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Linking Error");
		ignisPrintProgramLog(program);
		glDeleteProgram(program);
		return IGNIS_FAILURE;
	}

	glValidateProgram(program);

	result = GL_FALSE;
	glGetProgramiv(program, GL_VALIDATE_STATUS, &result);
	if (result == GL_FALSE)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Validating Error");
		ignisPrintProgramLog(program);
		glDeleteProgram(program);

		return IGNIS_FAILURE;
	}

	return program;
}

GLuint ignisCompileShader(GLenum type, const char* source)
{
	if (source[0] == '\0')
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Shader source is missing for %s", ignisGetShaderType(type));
		return IGNIS_FAILURE;
	}

	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint result = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Compiling Error (%s)", ignisGetShaderType(type));
		ignisPrintShaderLog(shader);
		glDeleteShader(shader);

		return IGNIS_FAILURE;
	}

	return shader;
}

void ignisPrintShaderLog(GLuint shader)
{
	if (!glIsShader(shader))
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Failed to log: Object is not a shader");
		return;
	}

	GLint log_length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
	char* log_buffer = (char*)malloc(log_length);
	glGetShaderInfoLog(shader, log_length, &log_length, log_buffer);

	_ignisErrorCallback(IGNIS_ERROR, "[SHADER] %.*s", log_length, log_buffer);

	free(log_buffer);
}

void ignisPrintProgramLog(GLuint program)
{
	if (!glIsProgram(program))
	{
		_ignisErrorCallback(IGNIS_ERROR, "[SHADER] Failed to log: Object is not a program");
		return;
	}

	GLint log_length = 0;
	glGetShaderiv(program, GL_INFO_LOG_LENGTH, &log_length);
	char* log_buffer = (char*)malloc(log_length);
	glGetShaderInfoLog(program, log_length, &log_length, log_buffer);

	_ignisErrorCallback(IGNIS_ERROR, "[SHADER] %.*s", log_length, log_buffer);

	free(log_buffer);
}

const char* ignisGetShaderType(GLenum type)
{
	switch (type)
	{
	case GL_COMPUTE_SHADER:
		return "GL_COMPUTE_SHADER";
	case GL_VERTEX_SHADER:
		return "GL_VERTEX_SHADER";
	case GL_TESS_CONTROL_SHADER:
		return "GL_TESS_CONTROL_SHADER";
	case GL_TESS_EVALUATION_SHADER:
		return "GL_TESS_EVALUATION_SHADER";
	case GL_GEOMETRY_SHADER:
		return "GL_GEOMETRY_SHADER";
	case GL_FRAGMENT_SHADER:
		return "GL_FRAGMENT_SHADER";
	}

	return "INVALID_SHADER_TYPE";
}