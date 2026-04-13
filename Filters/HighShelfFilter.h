#pragma once
#ifndef HIGH_SHELF_FILTER_H
#define HIGH_SHELF_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class HighShelfFilter : public BiquadCutoffShelfBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadCutoffShelfBasedFilter<T, SampleRate, StageCount, Design> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsHighShelf();
	}
};

#endif