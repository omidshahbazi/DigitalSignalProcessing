#pragma once
#ifndef LOG_H
#define LOG_H

#include "Bitwise.h"
#include "FixedFunction.h"
#include <stdio.h>

class Log
{
public:
	enum class Types
	{
		None = 0,
		Debug = 1,
		Info = 2,
		Warning = 4,
		Error = 8,
		Critical = 16,
		General = Info | Warning | Error | Critical,
		All = Debug | Info | Warning | Error | Critical
	};

	typedef FixedFunction<void(Types Type, cstr Message)> Callback;

public:
	static void Initialize(Callback Callback);
	static void SetMask(Types Mask);

	template <typename... ArgsT>
	static void Write(Types Type, cstr Tag, cstr FormattedMessage, ArgsT... Args)
	{
		if (s_Callback == nullptr)
			return;

		if (!Bitwise::IsEnabled(s_Mask, Type))
			return;

		if (FormattedMessage == nullptr)
			return;

		const int16_t SIZE = 512;
		static char buff[SIZE];

		int16_t index = 0;

		if (Tag != nullptr)
			index += snprintf(buff + index, SIZE - index, "[%s] ", Tag);

		index += snprintf(buff + index, SIZE - index, FormattedMessage, Args...);

		buff[index++] = '\0';

		s_Callback(Type, buff);
	}

	template <typename... ArgsT>
	static void WriteDebug(cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Debug, nullptr, FormattedMessage, Args...);
	}

	template <typename... ArgsT>
	static void WriteDebug(cstr Tag, cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Debug, Tag, FormattedMessage, Args...);
	}

	template <typename... ArgsT>
	static void WriteInfo(cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Info, nullptr, FormattedMessage, Args...);
	}

	template <typename... ArgsT>
	static void WriteInfo(cstr Tag, cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Info, Tag, FormattedMessage, Args...);
	}

	template <typename... ArgsT>
	static void WriteWarning(cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Warning, nullptr, FormattedMessage, Args...);
	}

	template <typename... ArgsT>
	static void WriteWarning(cstr Tag, cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Warning, Tag, FormattedMessage, Args...);
	}

	template <typename... ArgsT>
	static void WriteError(cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Error, nullptr, FormattedMessage, Args...);
	}

	template <typename... ArgsT>
	static void WriteError(cstr Tag, cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Error, Tag, FormattedMessage, Args...);
	}

	template <typename... ArgsT>
	static void WriteCritical(cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Critical, nullptr, FormattedMessage, Args...);
	}

	template <typename... ArgsT>
	static void WriteCritical(cstr Tag, cstr FormattedMessage, ArgsT... Args)
	{
		Write(Types::Critical, Tag, FormattedMessage, Args...);
	}

private:
	static Callback s_Callback;
	static Types s_Mask;
};

#endif