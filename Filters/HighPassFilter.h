#pragma once
#ifndef HIGH_PASS_FILTER_H
#define HIGH_PASS_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class HighPassFilter : public BiquadCutoffPassBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadCutoffPassBasedFilter<T, SampleRate, StageCount, Design> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsHighPass();
	}
};

#endif