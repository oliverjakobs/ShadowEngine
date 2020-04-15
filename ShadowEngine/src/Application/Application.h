#ifndef APPLICATION_H
#define APPLICATION_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Timer.h"
#include "Event/Event.h"

/* Graphics */
#include "Graphics/Renderer.h"
#include "Graphics/ResourceManager.h"

#include "Input.h"
#include <GLFW/glfw3.h>

typedef struct Application
{
	GLFWwindow* window;

	char* title;

	int width;
	int height;

	int running;
	int paused;

	int debug;
	int vsync;

	int show_gui;

	Timer timer;
	ResourceManager resources;

	mat4 screen_projection;

	void (*on_event)(struct Application*, const Event);
	void (*on_update)(struct Application*, float);
	void (*on_render)(struct Application*);
	void (*on_render_debug)(struct Application*);
	void (*on_render_gui)(struct Application*);
} Application;

int ApplicationLoad(Application* app, const char* title, int width, int height, int glMajor, int glMinor);
int ApplicationLoadConfig(Application* app, const char* path);
void ApplicationDestroy(Application* app);

/* --------------------------| Game Loop |------------------------------- */
void ApplicationRun(Application* app);
void ApplicationPause(Application* app);
void ApplicationClose(Application* app);

void ApplicationSetOnEventCallback(Application* app, void (*callback)(Application*, const Event));
void ApplicationSetOnUpdateCallback(Application* app, void (*callback)(Application*, float));
void ApplicationSetOnRenderCallback(Application* app, void (*callback)(Application*));
void ApplicationSetOnRenderDebugCallback(Application* app, void (*callback)(Application*));
void ApplicationSetOnRenderGuiCallback(Application* app, void (*callback)(Application*));

void ApplicationSetViewport(Application* app, int x, int y, int w, int h);
const float* ApplicationGetScreenProjPtr(Application* app);

/* --------------------------| Settings |-------------------------------- */
void ApplicationEnableDebugMode(Application* app, int b);
void ApplicationEnableVsync(Application* app, int b);
void ApplicationShowGui(Application* app, int b);

void ApplicationToggleDebugMode(Application* app);
void ApplicationToggleVsync(Application* app);
void ApplicationToggleGui(Application* app);

void ApplicationSetWindowTitle(Application* app, const char* title);
void ApplicationSetWindowTitleFormat(Application* app, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* !APPLICATION_H */