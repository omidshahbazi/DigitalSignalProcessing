#pragma once
#ifndef LOW_SHELF_FILTER_H
#define LOW_SHELF_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate>
class LowShelfFilter : public BiquadCutoffShelfBasedFilter<T, 1, SampleRate>
{
private:
	typedef BiquadCutoffShelfBasedFilter<T, 1, SampleRate> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsLowShelf();
	}
};

#endif