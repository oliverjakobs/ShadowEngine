#ifndef EVENT_H
#define EVENT_H

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
	EVENT_UNKOWN = 0,
	/* window events*/
	EVENT_WINDOW_CLOSE,
	EVENT_WINDOW_RESIZE,
	EVENT_WINDOW_FOCUS,
	EVENT_WINDOW_LOST_FOCUS,
	EVENT_WINDOW_MOVED,
	/* key events */
	EVENT_KEY_PRESSED,
	EVENT_KEY_RELEASED,
	EVENT_KEY_TYPED,
	/* mouse events */
	EVENT_MOUSE_BUTTON_PRESSED,
	EVENT_MOUSE_BUTTON_RELEASED,
	EVENT_MOUSE_MOVED,
	EVENT_MOUSE_SCROLLED
} EventType;

typedef struct
{
	int keycode;
	int repeatcount;
} KeyEvent;

typedef struct
{
	float x, y;
} MouseEvent;

typedef struct
{
	int buttoncode;
} MouseButtonEvent;

typedef struct
{
	unsigned int width, height;
} WindowEvent;

typedef struct
{
	EventType type;
	union
	{
		KeyEvent key;
		MouseEvent mouse;
		MouseButtonEvent mousebutton;
		WindowEvent window;
	};
} Event;

#ifdef __cplusplus
}
#endif

#endif // !EVENT_H