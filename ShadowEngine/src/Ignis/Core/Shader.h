#ifndef IGNIS_SHADER_H
#define IGNIS_SHADER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "../glad/glad.h"

typedef struct
{
	GLuint program;
} IgnisShader;

int ignisCreateShadervf(IgnisShader* shader, const char* vert, const char* frag);
int ignisCreateShadervgf(IgnisShader* shader, const char* vert, const char* geom, const char* frag);
int ignisCreateShaderSrcvf(IgnisShader* shader, const char* vert, const char* frag);
int ignisCreateShaderSrcvgf(IgnisShader* shader, const char* vert, const char* geom, const char* frag);
void ignisDeleteShader(IgnisShader* shader);

void ignisUseShader(IgnisShader* shader);

GLint ignisGetUniformLocation(const IgnisShader* shader, const char* name);

void ignisSetUniform1i(const IgnisShader* shader, const char* name, int value);
void ignisSetUniform1f(const IgnisShader* shader, const char* name, float value);
void ignisSetUniform2f(const IgnisShader* shader, const char* name, const float* values);
void ignisSetUniform3f(const IgnisShader* shader, const char* name, const float* values);
void ignisSetUniform4f(const IgnisShader* shader, const char* name, const float* values);
void ignisSetUniformMat2(const IgnisShader* shader, const char* name, const float* values);
void ignisSetUniformMat3(const IgnisShader* shader, const char* name, const float* values);
void ignisSetUniformMat4(const IgnisShader* shader, const char* name, const float* values);

void ignisSetUniform1iv(const IgnisShader* shader, const char* name, GLsizei count, const int* values);
void ignisSetUniform1fv(const IgnisShader* shader, const char* name, GLsizei count, const float* values);

/* unchecked location */
void ignisSetUniform1il(const IgnisShader* shader, GLint location, int value);
void ignisSetUniform1fl(const IgnisShader* shader, GLint location, float value);
void ignisSetUniform2fl(const IgnisShader* shader, GLint location, const float* values);
void ignisSetUniform3fl(const IgnisShader* shader, GLint location, const float* values);
void ignisSetUniform4fl(const IgnisShader* shader, GLint location, const float* values);
void ignisSetUniformMat2l(const IgnisShader* shader, GLint location, const float* values);
void ignisSetUniformMat3l(const IgnisShader* shader, GLint location, const float* values);
void ignisSetUniformMat4l(const IgnisShader* shader, GLint location, const float* values);

void ignisSetUniform1ivl(const IgnisShader* shader, GLint location, GLsizei count, const int* values);
void ignisSetUniform1fvl(const IgnisShader* shader, GLint location, GLsizei count, const float* values);

GLuint ignisCreateShaderProgram(GLenum* types, const char** sources, size_t count);
GLuint ignisCompileShader(GLenum type, const char* source);

void ignisPrintShaderLog(GLuint shader);
void ignisPrintProgramLog(GLuint program);

const char* ignisGetShaderType(GLenum type);

#ifdef __cplusplus
}
#endif

#endif /* !IGNIS_SHADER_H */