#include "Input.h"


#define INPUT_NUM_KEYS  (KEY_LAST + 1)

typedef struct
{
	int state;
	int prev;
} InputKeyState;

static InputKeyState key_states[INPUT_NUM_KEYS];

void InputUpdate(GLFWwindow* context)
{
	for (size_t i = KEY_SPACE; i < INPUT_NUM_KEYS; i++)
	{
		key_states[i].prev = key_states[i].state;
		key_states[i].state = (glfwGetKey(context, i) == GLFW_PRESS);
	}
}

int InputKeyPressed(int keycode)
{
	if (keycode > KEY_LAST || keycode == KEY_UNKNOWN)
		return 0;

	int state = glfwGetKey(glfwGetCurrentContext(), keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

int InputKeyReleased(int keycode)
{
	if (keycode > KEY_LAST || keycode == KEY_UNKNOWN)
		return 0;

	int state = glfwGetKey(glfwGetCurrentContext(), keycode);
	return state == GLFW_RELEASE;
}

int InputKeyHit(int keycode)
{
	if (keycode > KEY_LAST || keycode == KEY_UNKNOWN)
		return 0;

	return key_states[keycode].state && !key_states[keycode].prev;
}

int InputKeyDown(int keycode)
{
	if (keycode > KEY_LAST || keycode == KEY_UNKNOWN)
		return 0;

	return key_states[keycode].state;
}

int InputKeyUp(int keycode)
{
	if (keycode > KEY_LAST || keycode == KEY_UNKNOWN)
		return 0;

	return key_states[keycode].prev && !key_states[keycode].state;
}

int InputMousePressed(int button)
{
	if (button > MOUSE_BUTTON_LAST || button < MOUSE_BUTTON_1)
		return 0;

	int state = glfwGetMouseButton(glfwGetCurrentContext(), button);
	return state == GLFW_PRESS;
}

int InputMouseReleased(int button)
{
	if (button > MOUSE_BUTTON_LAST || button < MOUSE_BUTTON_1)
		return 0;

	int state = glfwGetMouseButton(glfwGetCurrentContext(), button);
	return state == GLFW_RELEASE;
}

void InputMousePosition(float* x, float* y)
{
	double xpos, ypos;
	glfwGetCursorPos(glfwGetCurrentContext(), &xpos, &ypos);

	if (x) *x = (float)xpos;
	if (y) *y = (float)ypos;
}

float InputMouseX()
{
	float x;

	InputMousePosition(&x, NULL);

	return x;
}

float InputMouseY()
{
	float y;

	InputMousePosition(NULL, &y);

	return y;
}

vec2 InputMousePositionVec2()
{
	vec2 vec;
	InputMousePosition(&vec.x, &vec.y);
	return vec;
}