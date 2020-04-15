#include "Renderer2D.h"

#include "Ignis/Quad.h"

typedef struct
{
	IgnisQuad quad;

	IgnisShader default_shader;
	IgnisShader* shader;

	GLint uniform_location_view_proj;
	GLint uniform_location_color;
	GLint uniform_location_model;
} _Renderer2DStorage;

static _Renderer2DStorage _render_data;

void Renderer2DInit(const char* vert, const char* frag)
{
	ignisCreateQuadTextured(&_render_data.quad);

	ignisShadervf(&_render_data.default_shader, vert, frag);

	Renderer2DSetShader(NULL);
}

void Renderer2DDestroy()
{
	ignisDeleteShader(&_render_data.default_shader);
	ignisDeleteQuad(&_render_data.quad);
}

void Renderer2DSetShader(IgnisShader* shader)
{
	if (shader)
	{
		_render_data.shader = shader;
	}
	else
	{
		_render_data.shader = &_render_data.default_shader;
	}

	_render_data.uniform_location_view_proj = ignisGetUniformLocation(_render_data.shader, "u_ViewProjection");
	// _render_data.uniform_location_color = ignisGetUniformLocation(_render_data.shader, "u_Color");
	_render_data.uniform_location_model = ignisGetUniformLocation(_render_data.shader, "u_Model");

	ignisSetUniform1i(_render_data.shader, "u_Texture", 0);
}

void Renderer2DRenderTexture(IgnisTexture* texture, float x, float y, float w, float h, const float* view_proj)
{
	Renderer2DRenderTextureC(texture, x, y, w, h, view_proj, IGNIS_WHITE);
}

void Renderer2DRenderTextureC(IgnisTexture* texture, float x, float y, float w, float h, const float* view_proj, IgnisColorRGBA color)
{
	/* creating a mat4 that translates and scales */
	float model[16];
	model[0] = w;		model[4] = 0.0f;	model[8] = 0.0f;	model[12] = x;
	model[1] = 0.0f;	model[5] = h;		model[9] = 0.0f;	model[13] = y;
	model[2] = 0.0f;	model[6] = 0.0f;	model[10] = 1.0f;	model[14] = 0.0f;
	model[3] = 0.0f;	model[7] = 0.0f;	model[11] = 0.0f;	model[15] = 1.0f;

	Renderer2DRenderTextureM(texture, model, view_proj, &color.r);
}

void Renderer2DRenderTextureM(IgnisTexture* texture, const float* model, const float* view_proj, const float* color)
{
	ignisUseShader(_render_data.shader);

	ignisSetUniformMat4l(_render_data.shader, _render_data.uniform_location_view_proj, view_proj);
	// ignisSetUniform4fl(_render_data.shader, _render_data.uniform_location_color, color);
	ignisSetUniformMat4l(_render_data.shader, _render_data.uniform_location_model, model);

	ignisBindTexture(texture, 0);

	ignisDrawQuadElements(&_render_data.quad, GL_TRIANGLES);
}
