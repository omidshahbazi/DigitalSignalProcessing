#pragma once
#ifndef I_DSP_H
#define I_DSP_H

#include "../Common.h"

template <typename T>
class IDSP
{
	static_assert(ARE_TYPES_THE_SAME(T, float) || ARE_TYPES_THE_SAME(T, double) "T must be float or double");

public:
	virtual void ProcessBuffer(T *Buffer, uint16 Count) = 0;
};

#endif