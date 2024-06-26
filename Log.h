#pragma once
#ifndef LOG_H
#define LOG_H

#include "Bitwise.h"
#include "IHAL.h"
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

public:
	static void Initialize(IHAL *HAL)
	{
		GetHAL() = HAL;
	}

	static void SetMask(Types Mask)
	{
		GetMask() = Mask;
	}

	template <typename... ArgsT>
	static void Write(Types Type, cstr Tag, cstr FormattedMessage, ArgsT... Args)
	{
		if (GetHAL() == nullptr)
			return;

		if (!Bitwise::IsEnabled(GetMask(), Type))
			return;

		if (FormattedMessage == nullptr)
			return;

		const int16 SIZE = 512;
		static char buff[SIZE];

		int16 index = 0;

		buff[index++] = '[';

		if (Bitwise::IsEnabled(Type, Types::Debug))
			buff[index++] = 'D';

		if (Bitwise::IsEnabled(Type, Types::Info))
			buff[index++] = 'I';

		if (Bitwise::IsEnabled(Type, Types::Warning))
			buff[index++] = 'W';

		if (Bitwise::IsEnabled(Type, Types::Error))
			buff[index++] = 'E';

		if (Bitwise::IsEnabled(Type, Types::Critical))
			buff[index++] = 'C';

		buff[index++] = ']';

		if (Tag != nullptr)
			index += snprintf(buff + index, SIZE - index, "[%s] ", Tag);

		index += snprintf(buff + index, SIZE - index, FormattedMessage, Args...);

		buff[index++] = '\n';
		buff[index++] = '\0';

		GetHAL()->Print(buff);
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

	template <typename... ArgsT>
	static void Break(cstr Tag, cstr FormattedMessage, ArgsT... Args)
	{
		WriteCritical(Tag, FormattedMessage, Args...);

		GetHAL()->Break();
	}

private:
	static IHAL *&GetHAL(void)
	{
		static IHAL *hal;

		return hal;
	}

	static Types &GetMask(void)
	{
		static Types mask = Types::None;

		return mask;
	}
};

#endif