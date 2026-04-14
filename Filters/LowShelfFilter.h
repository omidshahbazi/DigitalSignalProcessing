#pragma once
#ifndef LOW_SHELF_FILTER_H
#define LOW_SHELF_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount = 1, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class LowShelfFilter : public BiquadCutoffShelfBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadCutoffShelfBasedFilter<T, SampleRate, StageCount, Design> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsLowShelf();
	}
};

#endif