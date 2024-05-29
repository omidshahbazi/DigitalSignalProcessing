#pragma once
#ifndef I_DSP_H
#define I_DSP_H

#include "../Common.h"

template <typename T, uint32 SampleRate>
class IDSP
{
	static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double), "T must be float or double");
	static_assert(MIN_SAMPLE_RATE <= SampleRate && SampleRate <= MAX_SAMPLE_RATE, "Invalid SampleRate");

public:
	virtual void ProcessBuffer(T *Buffer, uint8 Count) = 0;
};

#endif