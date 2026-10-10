#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include "Log.h"
#include "FixedFunction.h"

class Debug
{
public:
	typedef FixedFunction<void(void), 8> CrashHandler;

public:
	static void SetCrashHandler(CrashHandler Handler);

	static bool IsDebuggerPresent(void);

	static void Crash(void);

	static void Break(void);

private:
	static CrashHandler s_Handler;
};

#ifndef DEBUG_STRINGIZE
#define DEBUG_STRINGIZE_EXPANDED(Value) #Value
#define DEBUG_STRINGIZE(Value) DEBUG_STRINGIZE_EXPANDED(Value)
#endif

#ifdef DEBUG
#define ENABLE_ASSERTS
#endif

#ifdef ENABLE_ASSERTS

#include <cstdlib>

#ifdef ENABLE_BREAK_FILE_LINE_INFO

#define ASSERT(Expression, Message, ...)																		\
	do																											\
	{																											\
		if (Expression)																							\
			break;																								\
		Log::WriteCritical(__FILE__ ":Ln" DEBUG_STRINGIZE(__LINE__) ", " #Expression, Message, ##__VA_ARGS__);  \
		Debug::Crash();																							\
		std::abort();																							\
	} while (false)

#define BREAK(Message, ...)																	  \
	do																						  \
	{																						  \
		Log::WriteCritical(__FILE__ ":Ln" DEBUG_STRINGIZE(__LINE__), Message, ##__VA_ARGS__); \
		Debug::Crash();																		  \
		std::abort();																		  \
	} while (false)

#else

#define ASSERT(Expression, Message, ...)						 \
	do															 \
	{															 \
		if (Expression)											 \
			break;												 \
		Log::WriteCritical(#Expression, Message, ##__VA_ARGS__); \
		Debug::Crash();											 \
		std::abort();											 \
	} while (false)

#define BREAK(Message, ...)										\
	do															\
	{															\
		Log::WriteCritical("Break", Message, ##__VA_ARGS__);	\
		Debug::Crash();											\
		std::abort();											\
	} while (false)

#endif

#else

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
		std::abort();					\
	} while (false)

#endif

#define NOT_IMPLEMENTED() BREAK("Not Implemented.")

#endif