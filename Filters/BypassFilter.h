#pragma once
#ifndef BYPASS_FILTER_H
#define BYPASS_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32 SampleRate, uint8 StageCount = 1, BiquadFilterDesign Design = BiquadFilterDesign::Identical>
class BypassFilter : protected BiquadBasedFilter<T, SampleRate, StageCount, Design>
{
};

#endif