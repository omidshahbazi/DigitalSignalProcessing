#pragma once
#ifndef GAIN_H
#define GAIN_H

#include "Math.h"

struct dBGain;

struct LinearGain
{
public:
	constexpr LinearGain(void) = default;
	constexpr explicit LinearGain(float Value)
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
	constexpr dBGain(void) = default;
	constexpr explicit dBGain(float Value)
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
static constexpr int8_t MIN_GAIN_VALUE = -90;
static constexpr dBGain MIN_GAIN(MIN_GAIN_VALUE);

static constexpr int8_t SILENCE_GAIN_VALUE = -40;
static constexpr dBGain SILENCE_GAIN(SILENCE_GAIN_VALUE);

static constexpr int8_t NORMAL_GAIN_VALUE = 0;
static constexpr dBGain NORMAL_GAIN(NORMAL_GAIN_VALUE);

static constexpr int8_t MAX_GAIN_VALUE = 6;
static constexpr dBGain MAX_GAIN(MAX_GAIN_VALUE);

#endif