#pragma once
#ifndef PEAK_EQ_FILTER_H
#define PEAK_EQ_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class PeakEQFilter : public BiquadEQBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadEQBasedFilter<T, SampleRate, StageCount, Design> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsPeakEQ();
	}
};

#endif