#pragma once
#ifndef TILT_EQ_FILTER_H
#define TILT_EQ_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class TiltEQFilter : public BiquadEQBasedFilter<T, SampleRate, StageCount, Design>
{
private:
	typedef BiquadEQBasedFilter<T, SampleRate, StageCount, Design> Base;

private:
	void UpdateCoefficients(void) override
	{
		Base::SetAsTiltEQ();
	}
};

#endif