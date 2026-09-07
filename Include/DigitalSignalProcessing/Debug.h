#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include "Log.h"

#ifndef DEBUG_STRINGIZE
#define DEBUG_STRINGIZE_EXPANDED(Value) #Value
#define DEBUG_STRINGIZE(Value) DEBUG_STRINGIZE_EXPANDED(Value)
#endif

#ifndef DEBUG
#define DISABLE_ASSERTS
#endif

#ifdef DISABLE_ASSERTS

#define ASSERT(Expression, Message, ...) \
	do                                   \
	{                                    \
		(void)sizeof(Expression);        \
		(void)sizeof(Message);           \
	} while (false)

#define BREAK(Message, ...)				\
	do                                  \
	{                                   \
		(void)sizeof(Message);			\
                                        \
	} while (false)

#else

#ifdef ENABLE_BREAK_FILE_LINE_INFO

#include <cstdlib>

#define ASSERT(Expression, Message, ...)                                                                \
	do                                                                                                  \
	{                                                                                                   \
		if (Expression)                                                                                 \
			break;                                                                                      \
		Log::Break(__FILE__ ":Ln" DEBUG_STRINGIZE(__LINE__) ", " #Expression, Message, ##__VA_ARGS__);  \
		std::abort();																					\
                                                                                                        \
	} while (false)

#define BREAK(Message, ...)																\
	do                                                                                  \
	{                                                                                   \
		Log::Break(__FILE__ ":Ln" DEBUG_STRINGIZE(__LINE__), Message, ##__VA_ARGS__);	\
		std::abort();																	\
                                                                                        \
	} while (false)

#else

#define ASSERT(Expression, Message, ...)                 \
	do                                                   \
	{                                                    \
		if (Expression)                                  \
			break;                                       \
		Log::Break(#Expression, Message, ##__VA_ARGS__); \
		std::abort();									 \
                                                         \
	} while (false)

#define BREAK(Message, ...)								\
	do                                                  \
	{                                                   \
		Log::Break("Break", Message, ##__VA_ARGS__);	\
		std::abort();									\
                                                        \
	} while (false)

#endif
#endif

#define NOT_IMPLEMENTED() BREAK("Not Implemented.")

#endif