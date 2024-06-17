#pragma once
#ifndef FILTER_H
#define FILTER_H

#include "../Common.h"

template <typename T, uint32 SampleRate>
class Filter
{
	ASSERT_ON_FLOATING_TYPE(T);
	ASSERT_ON_SAMPLE_RATE(SampleRate);

public:
	virtual T Process(T Value) = 0;

	virtual void ProcessBuffer(T *Buffer, uint8 Count)
	{
		for (uint16 i = 0; i < Count; ++i)
			Buffer[i] = Process(Buffer[i]);
	}
};

#endif