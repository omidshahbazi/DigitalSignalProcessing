#pragma once
#ifndef LOW_PASS_FILTER_H
#define LOW_PASS_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class LowPassFilter : public BiquadCutoffPassBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadCutoffPassBasedFilter<T, SampleRate, StageCount, Design> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsLowPass();
	}
};

#endif