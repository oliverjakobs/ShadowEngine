#include "Background.h"

#include "Graphics/Renderer.h"

int BackgroundInit(Background* background, size_t capacity)
{
	background->layers = (BackgroundLayer*)malloc(sizeof(BackgroundLayer) * capacity);
	background->size = 0;
	background->capacity = capacity;

	return background->layers != NULL;
}

void BackgroundClear(Background* background)
{
	free(background->layers);
	background->size = 0;
	background->capacity = 0;
}

int BackgroundPushLayer(Background* background, IgnisTexture* texture, float x, float y, float w, float h, float parallax)
{
	if (background->size >= background->capacity)
		return 0;

	background->layers[background->size].texture = texture;
	background->layers[background->size].startpos = x;
	background->layers[background->size].pos_x = x;
	background->layers[background->size].pos_y = y;
	background->layers[background->size].width = w;
	background->layers[background->size].height = h;
	background->layers[background->size].parallax = parallax;
	background->size++;
	return 1;
}

static void _LayerUpdate(BackgroundLayer* layer, float x, float deltatime)
{
	float rel_dist = x * (1 - layer->parallax);
	float dist = x * layer->parallax;

	layer->pos_x = layer->startpos + dist;

	if (rel_dist > layer->startpos + layer->width) layer->startpos += layer->width;
	else if (rel_dist < layer->startpos - layer->width) layer->startpos -= layer->width;
}

static void _LayerRender(BackgroundLayer* layer)
{
	BatchRenderer2DRenderTexture(layer->texture, layer->pos_x - layer->width, layer->pos_y, layer->width, layer->height);
	BatchRenderer2DRenderTexture(layer->texture, layer->pos_x, layer->pos_y, layer->width, layer->height);
	BatchRenderer2DRenderTexture(layer->texture, layer->pos_x + layer->width, layer->pos_y, layer->width, layer->height);
}

void BackgroundUpdate(Background* background, float x, float deltatime)
{
	for (size_t i = 0; i < background->size; i++)
		_LayerUpdate(&background->layers[i], x, deltatime);
}

void BackgroundRender(Background* background, const float* mat_view_proj)
{
	BatchRenderer2DStart(mat_view_proj);

	for (size_t i = 0; i < background->size; i++)
		_LayerRender(&background->layers[i]);

	BatchRenderer2DFlush();
}
