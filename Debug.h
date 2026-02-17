#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include "Log.h"

#ifndef STRINGIZE
#define STRINGIZE(Value) #Value
#endif
#define STRINGIZE_NUMBER(Value) STRINGIZE(Value)

#ifdef DEBUG
#ifdef USE_BREAK_ONLY_ASSERT
#define ASSERT(Expression, Message, ...)                 \
	do                                                   \
	{                                                    \
		if (Expression)                                  \
			break;                                       \
		Log::Break(#Expression, Message, ##__VA_ARGS__); \
                                                         \
	} while (false)
#else
#define ASSERT(Expression, Message, ...)                                                                \
	do                                                                                                  \
	{                                                                                                   \
		if (Expression)                                                                                 \
			break;                                                                                      \
		Log::Break(__FILE__ ":Ln" STRINGIZE_NUMBER(__LINE__) ", " #Expression, Message, ##__VA_ARGS__); \
                                                                                                        \
	} while (false)
#endif
#else
#define ASSERT(Expression, Message, ...) \
	do                                   \
	{                                    \
		Expression;                      \
	} while (false)
#endif

#endif