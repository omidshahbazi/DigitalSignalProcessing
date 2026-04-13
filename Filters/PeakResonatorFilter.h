#pragma once
#ifndef PEAK_RESONATOR_FILTER_H
#define PEAK_RESONATOR_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class PeakResonatorFilter : public BiquadBandBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadBandBasedFilter<T, SampleRate, StageCount, Design> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsPeakResonator();
	}
};

#endif