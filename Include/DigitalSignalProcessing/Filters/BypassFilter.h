#pragma once
#ifndef BYPASS_FILTER_H
#define BYPASS_FILTER_H

#include "BiquadFilter.h"

template <typename T, uint32_t SampleRate, uint8_t StageCount = 1, BiquadFilterDesigns Design = BiquadFilterDesigns::Identical>
class BypassFilter : protected BiquadBasedFilter<T, SampleRate, StageCount, Design>
{
};

#endif