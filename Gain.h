#pragma once
#ifndef GAIN_H
#define GAIN_H

#include "Math.h"

struct dBGain;

struct LinearGain
{
public:
	LinearGain(void) = default;
	LinearGain(float Value)
		: m_Value(Value)
	{
	}
	LinearGain(const dBGain &gain);

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 1.0f;
};

struct dBGain
{
public:
	dBGain(void) = default;
	dBGain(float Value)
		: m_Value(Value)
	{
	}
	dBGain(const LinearGain &gain)
	{
		m_Value = Math::LinearTodB(gain);
	}

	operator float(void) const
	{
		return m_Value;
	}

private:
	float m_Value = 1.0f;
};

inline LinearGain::LinearGain(const dBGain &gain)
{
	m_Value = Math::dBToLinear(gain);
}

#endif