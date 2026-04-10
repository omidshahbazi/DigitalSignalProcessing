#pragma once
#ifndef BAND_STOP_FILTER_H
#define BAND_STOP_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate>
class BandStopFilter : public BiquadBandBasedFilter<T, 1, SampleRate>
{
private:
	typedef BiquadBandBasedFilter<T, 1, SampleRate> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsBandStop();
	}
};

#endif