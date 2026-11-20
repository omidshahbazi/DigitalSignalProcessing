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

#define DSP_ASSERT(Expression, Message, ...)																		\
	do																											\
	{																											\
		if (Expression)																							\
			break;																								\
		Log::WriteCritical(__FILE__ ":Ln" DEBUG_STRINGIZE(__LINE__) ", " #Expression, Message, ##__VA_ARGS__);  \
		Debug::Crash();																							\
		std::abort();																							\
	} while (false)

#define DSP_BREAK(Message, ...)																	  \
	do																						  \
	{																						  \
		Log::WriteCritical(__FILE__ ":Ln" DEBUG_STRINGIZE(__LINE__), Message, ##__VA_ARGS__); \
		Debug::Crash();																		  \
		std::abort();																		  \
	} while (false)

#else

#define DSP_ASSERT(Expression, Message, ...)						 \
	do															 \
	{															 \
		if (Expression)											 \
			break;												 \
		Log::WriteCritical(#Expression, Message, ##__VA_ARGS__); \
		Debug::Crash();											 \
		std::abort();											 \
	} while (false)

#define DSP_BREAK(Message, ...)										\
	do															\
	{															\
		Log::WriteCritical("Break", Message, ##__VA_ARGS__);	\
		Debug::Crash();											\
		std::abort();											\
	} while (false)

#endif

#else

#define DSP_ASSERT(Expression, Message, ...) \
	do                                   \
	{                                    \
		(void)sizeof(Expression);        \
		(void)sizeof(Message);           \
	} while (false)

#define DSP_BREAK(Message, ...)				\
	do                                  \
	{                                   \
		(void)sizeof(Message);			\
		std::abort();					\
	} while (false)

#endif

#define ASSERT DSP_ASSERT
#define BREAK DSP_BREAK

#define NOT_IMPLEMENTED() BREAK("Not Implemented.")

#define ASSERT_NOT_NULL(Value) ASSERT(Value != nullptr, "Invalid value [nullptr]");

#define ASSERT_VALUE_COMPARE(Value, Op, Constraint)                          \
	do                                                                        \
	{                                                                         \
		const auto AssertV_ = (Value);                                        \
		const auto AssertC_ = (Constraint);                                   \
		ASSERT(AssertV_ Op AssertC_, "Expected [%f] " #Op " [%f]", static_cast<double>(AssertV_), static_cast<double>(AssertC_)); \
	} while (false)

#define ASSERT_VALUE_NOT_EQUAL(Value, Constraint)     ASSERT_VALUE_COMPARE(Value, !=, Constraint)
#define ASSERT_VALUE_GREATER(Value, Constraint)       ASSERT_VALUE_COMPARE(Value, >,  Constraint)
#define ASSERT_VALUE_GREATER_EQUAL(Value, Constraint) ASSERT_VALUE_COMPARE(Value, >=, Constraint)
#define ASSERT_VALUE_LESS(Value, Constraint)          ASSERT_VALUE_COMPARE(Value, <,  Constraint)
#define ASSERT_VALUE_LESS_EQUAL(Value, Constraint)    ASSERT_VALUE_COMPARE(Value, <=, Constraint)

#define ASSERT_VALUE_RANGE(Value, Min, OpMin, OpMax, Max)                    \
	do                                                                        \
	{                                                                         \
		const auto AssertV_   = (Value);                                      \
		const auto AssertMin_ = (Min);                                        \
		const auto AssertMax_ = (Max);                                        \
		ASSERT(AssertMin_ OpMin AssertV_ && AssertV_ OpMax AssertMax_, "Expected [%f] " #OpMin " [%f] " #OpMax " [%f]", static_cast<double>(AssertMin_), static_cast<double>(AssertV_), static_cast<double>(AssertMax_));                              \
	} while (false)

#define ASSERT_VALUE_RANGE_ALL_INCLUSIVE(Value, Min, Max) ASSERT_VALUE_RANGE(Value, Min, <=, <=, Max)
#define ASSERT_VALUE_RANGE_NON_INCLUSIVE(Value, Min, Max) ASSERT_VALUE_RANGE(Value, Min, <,  <,  Max)
#define ASSERT_VALUE_RANGE_MIN_INCLUSIVE(Value, Min, Max) ASSERT_VALUE_RANGE(Value, Min, <=, <,  Max)
#define ASSERT_VALUE_RANGE_MAX_INCLUSIVE(Value, Min, Max) ASSERT_VALUE_RANGE(Value, Min, <,  <=, Max)

#endif