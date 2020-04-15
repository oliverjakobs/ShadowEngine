#ifndef APPLICATION_CALLBACK_H
#define APPLICATION_CALLBACK_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Application.h"

// gets called when an event is processed, handles some (e.g. WindowCloseEvents) 
// and passes the rest to OnEvent, which is implemented in the game class
void ApplicationEventCallback(Application* app, const Event e);

void ApplicationIgnisErrorCallback(ignisErrorLevel level, const char* desc);

/* GLFW */
void ApplicationGLFWErrorCallback(int error, const char* desc);
void ApplicationGLFWWindowSizeCallback(GLFWwindow* window, int width, int height);
void ApplicationGLFWWindowCloseCallback(GLFWwindow* window);
void ApplicationGLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void ApplicationGLFWCharCallback(GLFWwindow* window, unsigned int keycode);
void ApplicationGLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ApplicationGLFWScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void ApplicationGLFWCursorPosCallback(GLFWwindow* window, double xPos, double yPos);

#ifdef __cplusplus
}
#endif

#endif /* !APPLICATION_CALLBACK_H */
