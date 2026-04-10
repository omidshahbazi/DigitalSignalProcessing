#pragma once
#ifndef LOW_PASS_FILTER_H
#define LOW_PASS_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate>
class LowPassFilter : public BiquadCutoffPassBasedFilter<T, 1, SampleRate>
{
private:
	typedef BiquadCutoffPassBasedFilter<T, 1, SampleRate> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsLowPass();
	}
};

#endif