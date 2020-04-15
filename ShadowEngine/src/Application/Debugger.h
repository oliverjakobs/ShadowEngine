#ifndef DEBUGGER_H
#define DEBUGGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#if _DEBUG

#define DEBUG_ENABLE_LOGGING
#define DEBUG_ENABLE_ASSERT

#endif

/*
 * -----------------------------------------------------------------------------------
 *                               Logging
 * -----------------------------------------------------------------------------------
 */
#ifdef DEBUG_ENABLE_LOGGING

#include <stdio.h>

#define DEBUG_INFO(s, ...)		printf(s, __VA_ARGS__)
#define DEBUG_TRACE(s, ...)		printf(s, __VA_ARGS__)
#define DEBUG_WARN(s, ...)		printf(s, __VA_ARGS__)
#define DEBUG_ERROR(s, ...)		printf(s, __VA_ARGS__)
#define DEBUG_CRITICAL(s, ...)	printf(s, __VA_ARGS__)

#else

#define DEBUG_INFO(s, ...)
#define DEBUG_TRACE(s, ...)
#define DEBUG_WARN(s, ...)
#define DEBUG_ERROR(s, ...)
#define DEBUG_CRITICAL(s, ...)

#endif
/*
 * -----------------------------------------------------------------------------------
 *                               Assert
 * -----------------------------------------------------------------------------------
 */
#ifdef DEBUG_ENABLE_ASSERT

#define DEBUG_ASSERT(x, ...) { if(!(x)) { DEBUG_CRITICAL("Assertion Failed: %s", __VA_ARGS__); __debugbreak(); } }

#else

#define DEBUG_ASSERT(x, ...)

#endif

#ifdef __cplusplus
}
#endif

#endif // !DEBUGGER_H
