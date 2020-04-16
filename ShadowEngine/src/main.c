#include "Application/Application.h"

#include "Shadow.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Camera camera;

ShadowEngine shadow;
Light mouse_light;

Light lights[4];

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

static void DebugRenderFrameBuffer(IgnisFrameBuffer* framebuffer, IgnisShader* shader, float x, float y, float w, float h, const float* view_proj)
{
	Renderer2DSetShader(shader);
	Renderer2DRenderTexture(&framebuffer->texture, x, h + y, w, -h, view_proj);
	Renderer2DSetShader(NULL);
}

static void DebugPrintFrameBuffer(const char* path, IgnisFrameBuffer* framebuffer)
{
	GLubyte* data = (GLubyte*)malloc(4 * (size_t)framebuffer->width * (size_t)framebuffer->height);
	if (data)
	{
		glReadPixels(0, 0, framebuffer->width, framebuffer->height, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_flip_vertically_on_write(1);
		stbi_write_bmp(path, framebuffer->width, framebuffer->height, 4, data);
	}
	free(data);
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
	ShadowEngineInit(&shadow);
	ShadowEngineCreateLight(&mouse_light, 0.0f, 0.0f, 512.0f, IGNIS_WHITE);

	ShadowEngineCreateLight(&lights[0], 160.0f, 240.0f, 256.0f, IGNIS_RED);
	ShadowEngineCreateLight(&lights[1], 860.0f, 240.0f, 512.0f, IGNIS_BLUE);
	ShadowEngineCreateLight(&lights[2], 620.0f, 540.0f, 512.0f, IGNIS_GREEN);
	ShadowEngineCreateLight(&lights[3], 240.0f, 640.0f, 512.0f, IGNIS_YELLOW);
}

void OnDestroy(Application* app)
{
	FontRendererDestroy();
	Primitives2DDestroy();
	BatchRenderer2DDestroy();
	Renderer2DDestroy();

	ShadowEngineDestroy(&shadow);
	ShadowEngineDeleteLight(&mouse_light);

	ShadowEngineDeleteLight(&lights[0]);
	ShadowEngineDeleteLight(&lights[1]);
	ShadowEngineDeleteLight(&lights[2]);
	ShadowEngineDeleteLight(&lights[3]);
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
	mouse_light.x = pos.x;
	mouse_light.y = pos.y;
}

void RenderLight(Light* light)
{
	ShadowEngineStart(&shadow, light);

	/* Render occluders */
	BatchRenderer2DStart(CameraGetViewProjectionPtr(&shadow.light_camera));

	for (size_t i = 0; i < SPRITE_COUNT; ++i)
	{
		Sprite* sprite = &sprites[i];
		BatchRenderer2DRenderTexture(sprite->texture, sprite->x, sprite->y, sprite->w, sprite->h);
	}

	BatchRenderer2DFlush();

	ShadowEngineProcess(&shadow, light, CameraGetViewProjectionPtr(&camera));

	ShadowEngineRender(&shadow, light, CameraGetViewProjectionPtr(&camera));
}

void OnRender(Application* app)
{
	RenderLight(&mouse_light);
	RenderLight(&lights[0]);
	RenderLight(&lights[1]);
	RenderLight(&lights[2]);
	RenderLight(&lights[3]);

	/* Render Scene */
	BatchRenderer2DStart(CameraGetViewProjectionPtr(&camera));

	for (size_t i = 0; i < SPRITE_COUNT; ++i)
	{
		Sprite* sprite = &sprites[i];
		BatchRenderer2DRenderTexture(sprite->texture, sprite->x, sprite->y, sprite->w, sprite->h);
	}

	BatchRenderer2DFlush();
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
	Application* app = (Application*)malloc(sizeof(Application));
	ApplicationLoadConfig(app, "config.json");

	OnInit(app);

	ApplicationSetOnEventCallback(app, OnEvent);
	ApplicationSetOnUpdateCallback(app, OnUpdate);
	ApplicationSetOnRenderCallback(app, OnRender);
	ApplicationSetOnRenderDebugCallback(app, OnRenderDebug);
	ApplicationSetOnRenderGuiCallback(app, OnRenderGui);

	ApplicationRun(app);

	ApplicationDestroy(app);

	OnDestroy(app);

	free(app);

	return 0;
}