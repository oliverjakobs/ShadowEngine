#ifndef BACKGROUND_H
#define BACKGROUND_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Ignis/Ignis.h"

typedef struct
{
	IgnisTexture2D* texture;

	float startpos;
	float pos_x;
	float pos_y;
	float width;
	float height;

	float parallax;
} BackgroundLayer;

typedef struct
{
	BackgroundLayer* layers;

	size_t size;
	size_t capacity;
} Background;

int BackgroundInit(Background* background, size_t capacity);
void BackgroundClear(Background* background);

int BackgroundPushLayer(Background* background, IgnisTexture2D* texture, float x, float y, float w, float h, float parallax);

void BackgroundUpdate(Background* background, float x, float deltatime);
void BackgroundRender(Background* background, const float* mat_view_proj);

#ifdef __cplusplus
}
#endif

#endif /* !BACKGROUND_H */