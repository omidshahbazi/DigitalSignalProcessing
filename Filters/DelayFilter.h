#pragma once
#ifndef DELAY_FILTER_H
#define DELAY_FILTER_H

#include "BufferFilter.h"

template <typename T, uint32 SampleRate, uint16 MaxTime>
class DelayFilter : public BufferFilter<T, SampleRate, MaxTime>
{
};

#endif