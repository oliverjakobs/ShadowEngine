#include "FontRenderer.h"

#include "Renderer.h"

typedef struct 
{
	IgnisVertexArray vao;
	IgnisShader shader;

	IgnisFont* font;
	IgnisColorRGBA color;

	float* vertices;
	size_t vertex_index;

	size_t quad_count;

	GLint uniform_location_proj;
	GLint uniform_location_color;
} _FontRendererStorage;

static _FontRendererStorage _render_data;

void FontRendererInit(const char* vert, const char* frag)
{
	ignisGenerateVertexArray(&_render_data.vao);

	IgnisBufferElement layout[] = { {GL_FLOAT, 4, GL_FALSE} };
	ignisAddArrayBufferLayout(&_render_data.vao, FONTRENDERER_BUFFER_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW, 0, layout, 1);

	GLuint indices[FONTRENDERER_INDEX_COUNT];
	GenerateIndices(indices, FONTRENDERER_INDEX_COUNT, FONTRENDERER_INDICES_PER_QUAD);

	ignisLoadElementBuffer(&_render_data.vao, indices, FONTRENDERER_INDEX_COUNT, GL_STATIC_DRAW);

	_render_data.vertices = (float*)malloc(BATCHRENDERER2D_BUFFER_SIZE * sizeof(float));
	_render_data.vertex_index = 0;
	_render_data.quad_count = 0;

	_render_data.font = NULL;
	_render_data.color = IGNIS_WHITE;

	ignisCreateShadervf(&_render_data.shader, vert, frag);

	_render_data.uniform_location_proj = ignisGetUniformLocation(&_render_data.shader, "u_Projection");
	_render_data.uniform_location_color = ignisGetUniformLocation(&_render_data.shader, "u_Color");
}

void FontRendererDestroy()
{
	free(_render_data.vertices);

	ignisDeleteShader(&_render_data.shader);

	ignisDeleteVertexArray(&_render_data.vao);
}

void FontRendererBindFont(IgnisFont* font, IgnisColorRGBA color)
{
	_render_data.font = font;
	_render_data.color = color;

	ignisSetUniform4fl(&_render_data.shader, _render_data.uniform_location_color, &_render_data.color.r);
}

void FontRendererStart(const float* mat_proj)
{
	ignisSetUniformMat4l(&_render_data.shader, _render_data.uniform_location_proj, mat_proj);
}

void FontRendererFlush()
{
	if (_render_data.vertex_index == 0)
		return;

	ignisBindFont(_render_data.font, 0);
	ignisBindVertexArray(&_render_data.vao);
	ignisBufferSubData(&_render_data.vao.array_buffers[0], 0, _render_data.vertex_index * sizeof(float), _render_data.vertices);

	ignisUseShader(&_render_data.shader);

	glDrawElements(GL_TRIANGLES, FONTRENDERER_INDICES_PER_QUAD * (GLsizei)_render_data.quad_count, GL_UNSIGNED_INT, NULL);

	_render_data.vertex_index = 0;
	_render_data.quad_count = 0;
}

void FontRendererRenderText(float x, float y, const char* text)
{
	if (!_render_data.font)
	{
		_ignisErrorCallback(IGNIS_WARN, "[FontRenderer] No font bound");
		return;
	}

	for (size_t i = 0; i < strlen(text); i++)
	{
		if (_render_data.vertex_index + FONTRENDERER_VERTICES_PER_QUAD * FONTRENDERER_VERTEX_SIZE >= FONTRENDERER_BUFFER_SIZE)
			FontRendererFlush();

		if (!ignisFontLoadCharQuad(_render_data.font, text[i], &x, &y, _render_data.vertices, _render_data.vertex_index))
			_ignisErrorCallback(IGNIS_WARN, "[FontRenderer] Failed to load quad for %c", text[i]);

		_render_data.vertex_index += FONTRENDERER_VERTICES_PER_QUAD * FONTRENDERER_VERTEX_SIZE;
		_render_data.quad_count++;
	}
}

void FontRendererRenderTextFormat(float x, float y, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t buffer_size = vsnprintf(NULL, 0, fmt, args);
	char* buffer = (char*)malloc(buffer_size + 1);
	vsnprintf(buffer, buffer_size + 1, fmt, args);
	va_end(args);

	FontRendererRenderText(x, y, buffer);

	free(buffer);
}
