#pragma once
#ifndef I_DSP_H
#define I_DSP_H

#include "../Common.h"

template <typename T, uint32_t SampleRate>
class IDSP
{
	ASSERT_ON_FLOATING_TYPE(T);
	ASSERT_ON_SAMPLE_RATE(SampleRate);

public:
	virtual void Process(T *Buffer, uint8_t Count) = 0;

protected:
	virtual T Mix(T A, T B)
	{
		return 0;
	}
};

#endif