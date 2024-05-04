#pragma once
#ifndef SAMPLE_AMOUNT_METER_H
#define SAMPLE_AMOUNT_METER_H

#include "Math.h"
#include "Log.h"

class SampleAmountMeter
{
public:
	SampleAmountMeter(void)
		: m_SampleCount(0),
		  m_SampleSum(0),
		  m_Average(0),
		  m_Min(0),
		  m_Max(0),
		  m_AbsoluteMax(0)
	{
	}

	void Record(float Value)
	{
		++m_SampleCount;

		m_SampleSum += Value;

		m_Average = m_SampleSum / m_SampleCount;

		if (Value < m_Min)
			m_Min = Value;
		else if (Value > m_Max)
			m_Max = Value;

		if (Math::Absolute(Value) > m_AbsoluteMax)
			m_AbsoluteMax = Math::Absolute(Value);
	}

	float GetAverage(void) const
	{
		return m_Average;
	}

	float GetMin(void) const
	{
		return m_Min;
	}

	float GetMax(void) const
	{
		return m_Max;
	}

	float GetAbsoluteMax(void) const
	{
		return m_AbsoluteMax;
	}

	void Reset(void)
	{
		m_SampleCount = 0;
		m_SampleSum = 0;
		m_Average = 0;
		m_Min = __FLT_MAX__;
		m_Max = __FLT_MIN__;
		m_AbsoluteMax = __FLT_MIN__;
	}

	void Print(void) const
	{
		Log::WriteInfo("Samples Min: %f Max: %f Average: %f Absolute Max: %f", m_Min, m_Max, m_Average, m_AbsoluteMax);
	}

private:
	uint32 m_SampleCount;
	double m_SampleSum;
	float m_Average;
	float m_Min;
	float m_Max;
	float m_AbsoluteMax;
};
#endif