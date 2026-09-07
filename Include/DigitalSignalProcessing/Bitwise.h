#pragma once
#ifndef BITWISE_H
#define BITWISE_H

#include "Common.h"

class Bitwise
{
public:
	template <typename T, typename U>
	static constexpr bool IsEnabled(T Mask, U Bits)
	{
		return (((int32_t)Mask & (int32_t)Bits) == (int32_t)Bits);
	}
};

template <typename T, typename U>
constexpr T operator|(T Left, U Right)
{
	return (T)((int32_t)Left | (int32_t)Right);
}

template <typename T, typename U>
constexpr T &operator|=(T &Left, U Right)
{
	Left = (T)((int32_t)Left | (int32_t)Right);
	return Left;
}

#endif