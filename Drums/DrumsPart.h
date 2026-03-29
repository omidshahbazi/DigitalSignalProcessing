#pragma once
#ifndef DRUMS_PART_H
#define DRUMS_PART_H

#include "../Common.h"

template <typename T, uint32 SampleRate>
class DrumsPart
{
	ASSERT_ON_FLOATING_TYPE(T);
	ASSERT_ON_SAMPLE_RATE(SampleRate);

public:
	virtual void Beat(void) = 0;

	virtual void Process(T *Buffer, uint8 Count) = 0;
};

#endif