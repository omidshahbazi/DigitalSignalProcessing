#pragma once
#ifndef GAIN_H
#define GAIN_H

#include "Math.h"

struct dBGain;

struct LinearGain
{
public:
	LinearGain(void) = default;
	explicit LinearGain(float Value)
		: m_Value(Value)
	{
	}
	LinearGain(const dBGain &gain);

	operator float(void) const
	{
		return m_Value;
	}

	LinearGain operator+(const LinearGain &Other)
	{
		return LinearGain(m_Value + Other.m_Value);
	}

	LinearGain operator-(const LinearGain &Other)
	{
		return LinearGain(m_Value - Other.m_Value);
	}

private:
	float m_Value = 1;
};

struct dBGain
{
public:
	dBGain(void) = default;
	explicit dBGain(float Value)
		: m_Value(Value)
	{
	}
	dBGain(const LinearGain& gain);

	operator float(void) const
	{
		return m_Value;
	}

	dBGain operator+(const dBGain &Other)
	{
		return dBGain(m_Value + Other.m_Value);
	}

	dBGain operator-(const dBGain &Other)
	{
		return dBGain(m_Value - Other.m_Value);
	}

private:
	float m_Value = 0;
};

// https://www.redcrab-software.com/en/Calculator/Electrics/Decibel-Factor
static const dBGain MIN_GAIN(-90);
static const dBGain SILENCE_GAIN(-40);
static const dBGain NORMAL_GAIN(0);
static const dBGain MAX_GAIN(6);

#endif