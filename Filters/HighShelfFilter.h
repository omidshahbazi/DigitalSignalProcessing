#pragma once
#ifndef HIGH_SHELF_FILTER_H
#define HIGH_SHELF_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate>
class HighShelfFilter : public BiquadCutoffShelfBasedFilter<T, 1, SampleRate>
{
private:
	typedef BiquadCutoffShelfBasedFilter<T, 1, SampleRate> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsHighShelf();
	}
};

#endif