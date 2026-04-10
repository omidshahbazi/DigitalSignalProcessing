#pragma once
#ifndef BAND_PASS_FILTER_H
#define BAND_PASS_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate>
class BandPassFilter : public BiquadBandBasedFilter<T, 1, SampleRate>
{
private:
	typedef BiquadBandBasedFilter<T, 1, SampleRate> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsBandPass();
	}
};

#endif