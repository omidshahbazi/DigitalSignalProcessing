#pragma once
#ifndef HIGH_PASS_FILTER_H
#define HIGH_PASS_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate>
class HighPassFilter : public BiquadCutoffPassBasedFilter<T, 1, SampleRate>
{
private:
	typedef BiquadCutoffPassBasedFilter<T, 1, SampleRate> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsHighPass();
	}
};

#endif