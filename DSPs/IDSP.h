#pragma once
#ifndef I_DSP_H
#define I_DSP_H

#include "../Common.h"
#include <type_traits>

template <typename T>
class IDSP
{
	static_assert(std::is_same<T, float>() || std::is_same<T, double>(), "T must be float or double");

public:
	virtual void ProcessBuffer(T *Buffer, uint16 Count) = 0;
};

#endif