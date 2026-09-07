#pragma once
#ifndef DELAY_FILTER_H
#define DELAY_FILTER_H

#include "BufferFilter.h"

template <typename T, uint32_t SampleRate, uint16_t MaxTime>
class DelayFilter : public BufferFilter<T, SampleRate, MaxTime>
{
};

#endif