#pragma once
#ifndef BAND_STOP_FILTER_H
#define BAND_STOP_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class BandStopFilter : public BiquadBandBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadBandBasedFilter<T, SampleRate, StageCount, Design> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsBandStop();
	}
};

#endif