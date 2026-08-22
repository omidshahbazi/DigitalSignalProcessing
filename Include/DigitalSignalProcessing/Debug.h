#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include "Log.h"

#ifndef STRINGIZE
#define DEBUG_STRINGIZE_EXPANDED(Value) #Value
#define DEBUG_STRINGIZE(Value) DEBUG_STRINGIZE_EXPANDED(Value)
#endif

#ifndef DEBUG
#define DISABLE_ASSERTS
#endif

#ifndef DEBUG
#define ENABLE_WAIT_FOR_DEBUGGER
#endif

#ifdef DISABLE_ASSERTS
#define ASSERT(Expression, Message, ...) \
	do                                   \
	{                                    \
		(void)sizeof(Expression);        \
		(void)sizeof(Message);           \
	} while (false)
#else
#ifdef USE_BREAK_ONLY_ASSERT
#define ASSERT(Expression, Message, ...)                 \
	do                                                   \
	{                                                    \
		if (Expression)                                  \
			break;                                       \
		Log::Break(#Expression, Message, ##__VA_ARGS__); \
		std::abort();									 \
                                                         \
	} while (false)
#else
#define ASSERT(Expression, Message, ...)                                                                \
	do                                                                                                  \
	{                                                                                                   \
		if (Expression)                                                                                 \
			break;                                                                                      \
		Log::Break(__FILE__ ":Ln" DEBUG_STRINGIZE(__LINE__) ", " #Expression, Message, ##__VA_ARGS__); \
		std::abort();																					\
                                                                                                        \
	} while (false)
#endif
#endif

#endif