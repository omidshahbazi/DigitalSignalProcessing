#pragma once
#ifndef RMS_CALCULATOR_FILTER_H
#define RMS_CALCULATOR_FILTER_H

#include "DelayFilter.h"

template <typename T, uint32 SampleRate, uint16 BufferLength>
class RMSCalculatorFilter : private DelayFilter<T, SampleRate, 1>
{
	static_assert(0 < BufferLength, "Invalid MaxTime");

public:
	RMSCalculatorFilter(void)
	{
		DelayFilter<T, SampleRate, 1>::SetTime(BufferLength / SampleRate);
	}

	T Process(T Value) override
	{
		return DelayFilter<T, SampleRate, 1>::Process(Value);
	}

	T Calculate(void)
	{
		T sumSquares = 0;

		for (uint16 i = 0; i < BufferLength; ++i)
		{
			T sample = DelayFilter<T, SampleRate, 1>::GetSample(i);

			sumSquares += sample * sample;
		}

		return Math::Root(sumSquares / BufferLength, 2);
	}

	T CalculateInverse(void)
	{
		T rms = Calculate();
		if (rms <= 0)
			return 0;

		return 1 / rms;
	}

	T CalculateGain(void)
	{
		return Math::Clamp(CalculateInverse(), 0.1, 10);
	}
};

#endif