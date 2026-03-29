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
	virtual void Process(T *Buffer, uint8 Count) = 0;
};

#endif