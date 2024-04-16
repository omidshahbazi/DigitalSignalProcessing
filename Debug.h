#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include "Log.h"

#define STRINGIZE(Value) #Value
#define STRINGIZE_NUMBER(Value) STRINGIZE(Value)

#ifdef _DEBUG
#define ASSERT(Expression, Message, ...)                                                                \
	do                                                                                                  \
	{                                                                                                   \
		if (Expression)                                                                                 \
			break;                                                                                      \
		Log::Break(__FILE__ ":Ln" STRINGIZE_NUMBER(__LINE__) ", " #Expression, Message, ##__VA_ARGS__); \
                                                                                                        \
	} while (false)
#else
#define ASSERT(Expression, Message, ...) \
	do                                   \
	{                                    \
		Expression;                      \
	} while (false)
#endif

#define CHECK_CALL(Expression) ASSERT(Expression, "CallFailed")

#endif