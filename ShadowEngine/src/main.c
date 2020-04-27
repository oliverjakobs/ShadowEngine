#include "Application/Application.h"

#include "Shadow.h"
#include "Camera/Camera.h"

#include "Ignis/ComputeShader.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Camera camera;

ShadowEngine shadow;

#define LIGHT_COUNT	5

static Light light;
static Light lights[LIGHT_COUNT];

typedef struct
{
	IgnisTexture2D* texture;

	float x;
	float y;
	float w;
	float h;
} Sprite;

#define SPRITE_COUNT 8

static Sprite sprites[SPRITE_COUNT];

static void DebugPrintFrameBuffer(const char* path, IgnisFrameBuffer* framebuffer)
{
	ignisBindFrameBuffer(framebuffer);
	GLubyte* data = (GLubyte*)malloc(4 * (size_t)framebuffer->width * (size_t)framebuffer->height);
	if (data)
	{
		glReadPixels(0, 0, framebuffer->width, framebuffer->height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_flip_vertically_on_write(1);
		stbi_write_bmp(path, framebuffer->width, framebuffer->height, 4, data);
	}
	free(data);
	ignisBindFrameBuffer(NULL);
}

static void DebugPrintTexture(const char* path, IgnisTexture2D* texture)
{
	ignisBindTexture2D(texture, 0);
	GLubyte* data = (GLubyte*)malloc(4 * (size_t)texture->width * (size_t)texture->height);
	if (data)
	{
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_flip_vertically_on_write(1);
		stbi_write_bmp(path, texture->width, texture->height, 4, data);
	}
	free(data);
	ignisBindTexture2D(NULL, 0);
}

void OnInit(Application* app)
{
	/* ---------------| Config |------------------------------------------ */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	Renderer2DInit("res/shaders/renderer2D.vert", "res/shaders/renderer2D.frag");
	Primitives2DInit("res/shaders/primitives.vert", "res/shaders/primitives.frag");
	BatchRenderer2DInit("res/shaders/batchrenderer.vert", "res/shaders/batchrenderer.frag");
	FontRendererInit("res/shaders/font.vert", "res/shaders/font.frag");

	FontRendererBindFont(ResourceManagerGetFont(&app->resources, "gui"), IGNIS_WHITE);

	ApplicationEnableDebugMode(app, 1);
	ApplicationEnableVsync(app, 0);
	ApplicationShowGui(app, 0);

	CameraCreateOrtho(&camera, (vec3) { app->width / 2.0f, app->height / 2.0f, 0.0f }, (vec2) { (float)app->width, (float)app->height });

	sprites[0] = (Sprite){ ResourceManagerGetTexture2D(&app->resources, "player"), 200.0f, 260.0f, 40.0f, 60.0f };
	sprites[1] = (Sprite){ ResourceManagerGetTexture2D(&app->resources, "box"), 100.0f, 140.0f, 128.0f, 64.0f };
	sprites[2] = (Sprite){ ResourceManagerGetTexture2D(&app->resources, "box"), 800.0f, 300.0f, 128.0f, 64.0f };
	sprites[3] = (Sprite){ ResourceManagerGetTexture2D(&app->resources, "box"), 600.0f, 600.0f, 128.0f, 64.0f };
	sprites[4] = (Sprite){ ResourceManagerGetTexture2D(&app->resources, "box"), 260.0f, 160.0f, 128.0f, 64.0f };
	sprites[5] = (Sprite){ ResourceManagerGetTexture2D(&app->resources, "box"), 140.0f, 500.0f, 128.0f, 64.0f };
	sprites[6] = (Sprite){ ResourceManagerGetTexture2D(&app->resources, "tree"), 400.0f, 580.0f, 48.0f, 128.0f };
	sprites[7] = (Sprite){ ResourceManagerGetTexture2D(&app->resources, "tree"), 600.0f, 400.0f, 48.0f, 128.0f };

	/* LIGHT */
	ShadowEngineInit(&shadow, app->width, app->height);

	ShadowEngineCreateLight(&light, 256.0f, 256.0f, 512.0f, IGNIS_WHITE);

	/*
	ShadowEngineCreateLight(&lights[0], 0.0f, 0.0f, 512.0f, IGNIS_WHITE);
	ShadowEngineCreateLight(&lights[1], 160.0f, 240.0f, 300.0f, IGNIS_RED);
	ShadowEngineCreateLight(&lights[2], 860.0f, 240.0f, 400.0f, IGNIS_BLUE);
	ShadowEngineCreateLight(&lights[3], 240.0f, 640.0f, 800.0f, IGNIS_YELLOW);
	ShadowEngineCreateLight(&lights[4], 620.0f, 540.0f, 512.0f, IGNIS_GREEN);
	*/
}

void OnDestroy(Application* app)
{
	FontRendererDestroy();
	Primitives2DDestroy();
	BatchRenderer2DDestroy();
	Renderer2DDestroy();

	ShadowEngineDestroy(&shadow);

	ShadowEngineDeleteLight(&light);

	/*
	ShadowEngineDeleteLight(&lights[0]);
	ShadowEngineDeleteLight(&lights[1]);
	ShadowEngineDeleteLight(&lights[2]);
	ShadowEngineDeleteLight(&lights[3]);
	ShadowEngineDeleteLight(&lights[4]);
	*/
}

void OnEvent(Application* app, const Event e)
{
	if (e.type == EVENT_WINDOW_RESIZE)
	{
		ApplicationSetViewport(app, 0, 0, e.window.width, e.window.height);
	}

	if (e.type == EVENT_KEY_PRESSED)
	{
		switch (e.key.keycode)
		{
		case KEY_ESCAPE:
			ApplicationClose(app);
			break;
		}
	}
}

void OnUpdate(Application* app, float deltaTime)
{
	vec2 pos = CameraGetMousePos(&camera, InputMousePositionVec2());
	light.x = pos.x;
	light.y = pos.y;
	/* 
	lights[0].x = pos.x;
	lights[0].y = pos.y;
	*/
}

void OnRender(Application* app)
{
	ShadowEngineStart(&shadow);

	/* Render occluders */
	ShadowEngineStartOcclusion(&shadow, CameraGetViewProjectionPtr(&camera));

	for (size_t i = 0; i < SPRITE_COUNT; ++i)
	{
		Sprite* sprite = &sprites[i];
		ShadowEngineRenderOccluder(&shadow, sprite->texture, sprite->x, sprite->y, sprite->w, sprite->h);
	}

	ShadowEngineFlushOcclusion(&shadow);

	/* Process lights */
	ShadowEngineProcess(&shadow, &light, 1, CameraGetViewProjectionPtr(&camera));

	/* Render Lights */
	ShadowEngineRender(&shadow, &light, 1, CameraGetViewProjectionPtr(&camera));
	
	ShadowEngineFinish(&shadow);

	// DebugPrintTexture("shadow.bmp", &light.shadow);

	// DebugPrintFrameBuffer("shadow_1.bmp", &light.shadow_map);
	// DebugPrintFrameBuffer("shadow_2.bmp", &light.shadow_map);

	/* Render Scene */
	// BatchRenderer2DStart(CameraGetViewProjectionPtr(&camera));
	// for (size_t i = 0; i < SPRITE_COUNT; ++i)
	// {
	// 	Sprite* sprite = &sprites[i];
	// 	BatchRenderer2DRenderTexture(sprite->texture, sprite->x, sprite->y, sprite->w, sprite->h);
	// }
	// BatchRenderer2DFlush();

	Renderer2DRenderTexture(&shadow.scene_buffer.texture, 0.0f, 0.0f, app->width, app->height, CameraGetViewProjectionPtr(&camera));
}

void OnRenderDebug(Application* app)
{
	/* fps */
	FontRendererStart(ApplicationGetScreenProjPtr(app));

	FontRendererRenderTextFormat(8.0f, 20.0f, "FPS: %d", app->timer.fps);

	FontRendererFlush();
}

void OnRenderGui(Application* app)
{
	
}

int main()
{
	Application app;
	ApplicationLoadConfig(&app, "config.json");

	OnInit(&app);

	ApplicationSetOnEventCallback(&app, OnEvent);
	ApplicationSetOnUpdateCallback(&app, OnUpdate);
	ApplicationSetOnRenderCallback(&app, OnRender);
	ApplicationSetOnRenderDebugCallback(&app, OnRenderDebug);
	ApplicationSetOnRenderGuiCallback(&app, OnRenderGui);

	ApplicationRun(&app);

	ApplicationDestroy(&app);

	OnDestroy(&app);

	return 0;
}