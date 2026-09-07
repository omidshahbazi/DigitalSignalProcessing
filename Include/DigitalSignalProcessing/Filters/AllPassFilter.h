#pragma once
#ifndef ALL_PASS_FILTER_H
#define ALL_PASS_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32_t SampleRate, uint8_t StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class AllPassFilter : public BiquadBandBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadBandBasedFilter<T, SampleRate, StageCount, Design> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsAllPass();
	}
};

#endif