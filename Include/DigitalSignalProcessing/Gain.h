#pragma once
#ifndef GAIN_H
#define GAIN_H

#include "Math.h"
#include "Debug.h"

struct dBGain;

struct LinearGain
{
public:
	constexpr LinearGain(void) = default;
	constexpr explicit LinearGain(float Value)
		: m_Value(Value)
	{
		ASSERT_VALUE_GREATER_EQUAL(m_Value, 0);
	}
	LinearGain(const dBGain &gain);

	constexpr operator float(void) const
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

	static LinearGain Interpolate(float Min, float Max, float Time, bool Linear = false)
	{
		return Interpolate(LinearGain(Min), LinearGain(Max), Time, Linear);
	}
	static LinearGain Interpolate(LinearGain Min, LinearGain Max, float Time, bool Linear = false);

private:
	float m_Value = 1;
};

struct dBGain
{
public:
	constexpr dBGain(void) = default;
	constexpr explicit dBGain(float Value)
		: m_Value(Value)
	{
	}
	dBGain(const LinearGain& gain);

	constexpr operator float(void) const
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

	static dBGain Interpolate(float Min, float Max, float Time)
	{
		return Interpolate(dBGain(Min), dBGain(Max), Time);
	}
	static dBGain Interpolate(dBGain Min, dBGain Max, float Time);

private:
	float m_Value = 0;
};

// https://www.redcrab-software.com/en/Calculator/Electrics/Decibel-Factor
static constexpr int8_t MIN_GAIN_VALUE = -90;
static constexpr dBGain MIN_GAIN(MIN_GAIN_VALUE);

static constexpr int8_t SILENCE_GAIN_VALUE = -40;
static constexpr dBGain SILENCE_GAIN(SILENCE_GAIN_VALUE);

static constexpr int8_t NORMAL_GAIN_VALUE = 0;
static constexpr dBGain NORMAL_GAIN(NORMAL_GAIN_VALUE);

static constexpr int8_t MAX_GAIN_VALUE = 6;
static constexpr dBGain MAX_GAIN(MAX_GAIN_VALUE);

#endif