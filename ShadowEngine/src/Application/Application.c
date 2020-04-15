#include "Application.h"

#include "Event/EventHandler.h"

#include "ApplicationCallback.h"

#include "json/tb_json.h"

#include "Debugger.h"
#include "defines.h"

int ApplicationLoad(Application* app, const char* title, int width, int height, int glMajor, int glMinor)
{
	app->title = malloc(strlen(title));
	strcpy(app->title, title);

	app->width = width;
	app->height = height;

	app->debug = 0;
	app->paused = 0;

	app->show_gui = 0;

	app->running = 0;

	/* GLFW initialization */
	if (glfwInit() == GLFW_FALSE)
	{
		DEBUG_ERROR("[GLFW] Failed to initialize GLFW\n");
		glfwTerminate();
		return 0;
	}

	DEBUG_INFO("[GLFW] Initialized GLFW %s\n", glfwGetVersionString());

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	/* creating the window */
	app->window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (app->window == NULL)
	{
		DEBUG_ERROR("[GLFW] Failed to create GLFW window\n");
		glfwTerminate();
		return 0;
	}

	glfwMakeContextCurrent(app->window);
	glfwSetWindowUserPointer(app->window, app);

	EventHandlerInit();
	EventHandlerSetEventCallback(ApplicationEventCallback);

	/* Set GLFW callbacks */
	glfwSetErrorCallback(ApplicationGLFWErrorCallback);
	glfwSetWindowSizeCallback(app->window, ApplicationGLFWWindowSizeCallback);
	glfwSetWindowCloseCallback(app->window, ApplicationGLFWWindowCloseCallback);
	glfwSetKeyCallback(app->window, ApplicationGLFWKeyCallback);
	glfwSetCharCallback(app->window, ApplicationGLFWCharCallback);
	glfwSetMouseButtonCallback(app->window, ApplicationGLFWMouseButtonCallback);
	glfwSetScrollCallback(app->window, ApplicationGLFWScrollCallback);
	glfwSetCursorPosCallback(app->window, ApplicationGLFWCursorPosCallback);

	ignisSetErrorCallback(ApplicationIgnisErrorCallback);

	int debug = 0;

#ifdef _DEBUG
	debug = 1;
#endif /* !_DEBUG */

	/* ingis initialization */
	if (!ignisInit(debug))
	{
		DEBUG_ERROR("[IGNIS] Failed to initialize Ignis\n");
		glfwTerminate();
		return 0;
	}

	DEBUG_INFO("[OpenGL] Version: %s\n", glGetString(GL_VERSION));
	DEBUG_INFO("[OpenGL] Vendor: %s\n", glGetString(GL_VENDOR));
	DEBUG_INFO("[OpenGL] Renderer: %s\n", glGetString(GL_RENDERER));
	DEBUG_INFO("[OpenGL] GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	ApplicationSetViewport(app, 0, 0, app->width, app->height);
	TimerReset(&app->timer);

	app->running = 1;
	return 1;
}

int ApplicationLoadConfig(Application* app, const char* path)
{
	char* json = ignisReadFile(path, NULL);

	if (!json)
	{
		DEBUG_ERROR("Failed to read config (%s)\n", path);
		return 0;
	}

	tb_json_element element;
	tb_json_read(json, &element, "{'app'");

	char title[APPLICATION_STR_LEN];
	tb_json_string((char*)element.value, "{'title'", title, APPLICATION_STR_LEN, NULL);

	int width = tb_json_int((char*)element.value, "{'width'", NULL);
	int height = tb_json_int((char*)element.value, "{'height'", NULL);

	int major = tb_json_int((char*)element.value, "{'opengl'[0", NULL);
	int minor = tb_json_int((char*)element.value, "{'opengl'[1", NULL);

	char index[APPLICATION_PATH_LEN];
	tb_json_string(json, "{'resources'", index, APPLICATION_PATH_LEN, NULL);

	free(json);

	if (ApplicationLoad(app, title, width, height, major, minor))
		return ResourceManagerInit(&app->resources, index);

	return 0;
}

void ApplicationDestroy(Application* app)
{
	ResourceManagerDestroy(&app->resources);

	EventHandlerDestroy();

	glfwDestroyWindow(app->window);
	glfwTerminate();
}

void ApplicationRun(Application* app)
{
	/* game loop */
	while (app->running)
	{
		TimerStart(&app->timer, glfwGetTime());
		InputUpdate(app->window);

		if (!app->paused)
			app->on_update(app, (float)app->timer.deltatime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		app->on_render(app);

		if (app->debug)
			app->on_render_debug(app);

		if (app->show_gui)
			app->on_render_gui(app);

		glfwPollEvents();
		EventHandlerPoll(app);
		glfwSwapBuffers(app->window);

		TimerEnd(&app->timer, glfwGetTime());
	}
}

void ApplicationPause(Application* app)
{
	app->paused = !app->paused;

	if (app->paused)
		ApplicationSetWindowTitleFormat(app, "%s | Paused", app->title);
	else
		ApplicationSetWindowTitle(app, app->title);
}

void ApplicationClose(Application* app) { app->running = 0; }

void ApplicationSetOnEventCallback(Application* app, void(*callback)(Application*, const Event))
{
	app->on_event = callback;
}

void ApplicationSetOnUpdateCallback(Application* app, void(*callback)(Application*, float))
{
	app->on_update = callback;
}

void ApplicationSetOnRenderCallback(Application* app, void(*callback)(Application*))
{
	app->on_render = callback;
}

void ApplicationSetOnRenderDebugCallback(Application* app, void(*callback)(Application*))
{
	app->on_render_debug = callback;
}

void ApplicationSetOnRenderGuiCallback(Application* app, void(*callback)(Application*))
{
	app->on_render_gui = callback;
}

void ApplicationSetViewport(Application* app, int x, int y, int w, int h)
{
	app->screen_projection = mat4_ortho((float)x, (float)w, (float)h, (float)y, -1.0f, 1.0f);
	glViewport(x, y, w, h);
}

const float* ApplicationGetScreenProjPtr(Application* app)
{
	return app->screen_projection.v;
}

void ApplicationEnableDebugMode(Application* app, int b) { app->debug = b; }
void ApplicationEnableVsync(Application* app, int b) { glfwSwapInterval(b); app->vsync = b; }
void ApplicationShowGui(Application* app, int b) { app->show_gui = b; }

void ApplicationToggleDebugMode(Application* app) { ApplicationEnableDebugMode(app, !app->debug); }
void ApplicationToggleVsync(Application* app) { ApplicationEnableVsync(app, !app->vsync); }
void ApplicationToggleGui(Application* app) { ApplicationShowGui(app, !app->show_gui); }

void ApplicationSetWindowTitle(Application* app, const char* title) 
{ 
	glfwSetWindowTitle(app->window, title);
}

void ApplicationSetWindowTitleFormat(Application* app, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	size_t buffer_size = vsnprintf(NULL, 0, fmt, args);
	char* buffer = (char*)malloc(buffer_size + 1);
	vsnprintf(buffer, buffer_size + 1, fmt, args);
	va_end(args);

	ApplicationSetWindowTitle(app, buffer);

	free(buffer);
}