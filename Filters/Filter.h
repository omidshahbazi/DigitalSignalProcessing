#pragma once
#ifndef FILTER_H
#define FILTER_H

#include "../Common.h"

template <typename T, uint32 SampleRate>
class Filter
{
	static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");
	static_assert(MIN_SAMPLE_RATE <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate");

public:
	virtual T Process(T Value) = 0;

	virtual void ProcessBuffer(T *Buffer, uint8 Count)
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = Process(Buffer[i]);
	}
};

#endif